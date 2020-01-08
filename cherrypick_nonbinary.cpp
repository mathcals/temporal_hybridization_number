#pragma once

#include <chrono>
#include "tree.h"
#include "common.h"
#include "cherrypick.h"
#include "cherrypick_semitemporal.h"

std::vector<tree_leaf> nonBinaryNeighbours(IndexedTree<tree_leaf>* tree, tree_leaf leaf) {
    auto parent = tree->parentMap[tree->labelMap[leaf]];

    if (parent == NULL) {
        // The leaf is the only element of the tree
        return std::vector<tree_leaf>();
    }

    std::vector<tree_leaf> ret;

    for (auto el: parent->children) {
        if (el->name != defaultValue<tree_leaf>() && el->name != leaf) {
            ret.push_back(el->name);
        }
    }
    return ret;
}

bool isInCherry(IndexedTree<tree_leaf>* tree, tree_leaf leaf) {
    auto parent = tree->parentMap[tree->labelMap[leaf]];

    if (parent == NULL) {
        // The leaf is the only element of the tree
        return false;
    }

    return parent->isCherry();
}


/**
 * Returns all cherries of a nonbinary tree.
 * @param tree
 * @return
 */
std::vector<std::vector<tree_leaf>> nonBinaryCherries(IndexedTree<tree_leaf>* tree) {
    std::vector<std::vector<tree_leaf>> ret;
    std::vector<Tree<tree_leaf>*> todo;
    todo.push_back(tree->tree);
    while (!todo.empty()) {
        auto elem = *todo.rbegin();
        todo.pop_back();
        if (elem->isCherry()) {
            std::vector<tree_leaf> leaves;
            for (auto &child : elem->children) {
                leaves.push_back(child->name);
            }
            ret.push_back(leaves);
        } else {
            for (auto &child : elem->children) {
                todo.push_back(child);
            }
        }
    }
    return ret;
}

/**
 * Repeatedly removes elements from trivial trees.
 * @param trees
 * @return
 */
std::vector<std::tuple<tree_leaf, short>> pickTrivial(std::vector<IndexedTree<tree_leaf>*> trees) {
    std::vector<std::tuple<tree_leaf, short>> sequence;
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto leaf : trees[0]->leaves()) {
            auto neighbours1 = nonBinaryNeighbours(trees[0], leaf);
            auto neighbours2 = nonBinaryNeighbours(trees[1], leaf);
            std::vector<tree_leaf> v3;

            std::sort(neighbours1.begin(), neighbours1.end());
            std::sort(neighbours2.begin(), neighbours2.end());

            std::set_intersection(neighbours1.begin(), neighbours1.end(),
                                  neighbours2.begin(), neighbours2.end(),
                                  back_inserter(v3));
            if (!v3.empty()) {
                trees[0]->remove(leaf);
                trees[1]->remove(leaf);
                sequence.push_back(std::make_tuple(leaf, 0));
                changed = true;
                break;
            }
        }
    }
    return sequence;
}

/**
 * Implementation of Algorithm 4
 * @param trees
 * @param k
 * @param timeout
 * @param returnSequence
 * @return
 */
CherryPickResult cherrypickNonBinaryTrees(std::vector<IndexedTree<tree_leaf>*> trees, int k, std::chrono::high_resolution_clock::time_point timeout, std::vector<std::tuple<tree_leaf, short>>& returnSequence) {
    if (trees.size() != 2)
        throw std::runtime_error("cherrypickNonBinaryTrees only works on two trees");
    if (k<0) {
        return CherryPickResult::no_solution_for_k;
    }
    CherryPickResult result = CherryPickResult::no_solution;
    if (std::chrono::high_resolution_clock::now() > timeout) {
        result = CherryPickResult::timeout;
        goto ret;
    }
    {
        auto trivialSequence = pickTrivial(trees);
        if (trees[0]->tree->children.empty()) {
            return CherryPickResult::success;
        }
        std::set<std::set<tree_leaf>> removalSets;
        for (auto tree: trees) {
            auto cherries = nonBinaryCherries(tree);
            if (cherries.size() > 3 * k) {
                return CherryPickResult::no_solution_for_k;
            }
            for (auto &cluster: cherries) {
                if (cluster.size() - 1 > k)
                    continue;
                for (auto it = cluster.begin(); it != cluster.end(); it++) {
                    std::set<tree_leaf> newElem;
                    std::copy_if(cluster.begin(), cluster.end(), std::inserter(newElem, newElem.begin()),
                                 [it](const tree_leaf &t) { return t != *it; });
                    removalSets.insert(newElem);
                }
            }
        }

        for (auto it = removalSets.begin();
        it != removalSets.end();
        ++it) {
            auto removalSet = *it;
            std::vector<IndexedTree<tree_leaf> *> newTrees;
            {
                bool validSet = true;
                for (auto x: removalSet) {
                    for (auto tree: trees) {
                        if (!isInCherry(tree, x)) {
                            validSet = false;
                            goto outOfLoop;
                        }
                    }
                }
                outOfLoop:
                if (!validSet) {
                    continue;
                }
            }
            for (auto tree: trees) {
                auto nTree = new IndexedTree(tree);
                for (auto x: removalSet) {
                    nTree->remove(x);
                }
                newTrees.push_back(nTree);
            }
            addResult(result, cherrypickNonBinaryTrees(newTrees, k - removalSet.size(), timeout, returnSequence));
            for (auto tree: newTrees) {
                delete tree;
            }
            if (result == CherryPickResult::success) {
                for (auto x: removalSet) {
                    returnSequence.insert(returnSequence.begin(), std::make_tuple(x, 1));
                }
                for (auto i = trivialSequence.rbegin(); i != trivialSequence.rend(); ++i)
                {
                    returnSequence.insert(returnSequence.begin(), *i);
                }
                goto ret;

            }
        }
    }


    ret:
    return result;
}
