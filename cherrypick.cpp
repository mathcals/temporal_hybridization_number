//
// Created by sander on 26-6-19.
//

#include <iostream>
#include "cherrypick.h"
#include <set>
#include <unordered_set>
#include <queue>
#include "cherrypick_semitemporal.h"
#include "cherrypick_nonbinary.h"
#include "util.h"
#include <chrono>
#include "cherrypick_nonbinary.h"
#include <chrono>

/**
 *
 * @param trees
 * @param algorithm
 * @param maxTemporalDistance Only used when algorithm = CherryPickAlgorithm::SemiTemporal
 * @param timeout_time
 * @param verbose
 * @param times
 * @return
 */
std::tuple<CherryPickResult, int> cherrypickRepeatedly(std::vector<Tree<tree_leaf> *> trees, CherryPickAlgorithm algorithm, int maxTemporalDistance, std::chrono::high_resolution_clock::time_point timeout_time, bool verbose, std::vector<float>& times, std::vector<std::tuple<tree_leaf,short>>& return_sequence, std::vector<std::tuple<tree_leaf,tree_leaf>>& treeChildSequence) {
    auto lastTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; true; i++) {
        std::vector<IndexedTree<tree_leaf> *> indexed;
        if (verbose)
            std::cout << "trying " << i << std::endl;
        for (auto &tree: trees) {
            indexed.push_back(new IndexedTree(new Tree(tree)));
        }
        reco_sequence<tree_leaf> recoSequence;
        subtreeReduce(indexed, recoSequence);

        auto new_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<float>>(new_time - lastTime).count();
        times.push_back(duration);

        lastTime = new_time;
        CherryPickResult result;
        
        switch (algorithm) {
            case Default: {
                constraint_set<tree_leaf> cset;
                auto in_all = inAllCherries(indexed);
                result = cherrypickDefault(indexed, cset, i,
                                           timeout_time, in_all, return_sequence);
                break;
            }
            case NonBinary: {
                result = cherrypickNonBinaryTrees(indexed, i, timeout_time, return_sequence);
                break;
            }
            case SemiTemporal: {
                constraint_set<tree_leaf> cset;
                auto in_all = inAllCherries(indexed);
                treeChildSequence.clear();
                result = cherrypickSemiTemporal(indexed, cset, i, i, maxTemporalDistance, std::set<tree_leaf>(),
                                                timeout_time, in_all, treeChildSequence);
                if (result == CherryPickResult::success) {
                    appendToReturnSequence(recoSequence, treeChildSequence);
                }
                break;
            }
        }
        for (auto &tree: indexed) {
            delete tree;
        }

        switch(result) {
            case CherryPickResult::timeout:
            case CherryPickResult::no_solution:
                return_sequence.clear();
            case CherryPickResult::success:
                return {result, i};
        }

    }
}


std::tuple<CherryPickResult, int, int> cherrypickSemiTemporalRepeatedly(std::vector<Tree<tree_leaf> *> trees, CherryPickAlgorithm algorithm, std::chrono::high_resolution_clock::time_point timeout_time, bool verbose, std::vector<std::tuple<tree_leaf,tree_leaf>>& treeChildSequence) {
    std::vector<std::tuple<tree_leaf,short>> return_sequence;
    for (int maxTemporalDistance = 0; true; maxTemporalDistance++) {
        std::vector<float> times;
        if (verbose)
        {
            std::cout << std::endl << "maxTemporalDistance=" << maxTemporalDistance << std::endl << std::endl;
        }
        auto result = cherrypickRepeatedly(trees, algorithm, maxTemporalDistance, timeout_time, verbose, times, return_sequence, treeChildSequence);
        switch(std::get<0>(result)) {
            case CherryPickResult::timeout:
                return_sequence.clear();
            case CherryPickResult::success:
                return {std::get<0>(result), std::get<1>(result), maxTemporalDistance};
        }

    }
}

void addResult(CherryPickResult &result, CherryPickResult newValue) {
    result = static_cast<CherryPickResult>(result & newValue);
}


