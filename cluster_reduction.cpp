//
// Created by sander on 04-10-19.
//

#include <iostream>
#include "cluster_reduction.h"
#include "util.h"
#include <boost/range/combine.hpp>
#include <tuple>

bool compareSize(std::pair<std::vector<tree_leaf>, std::vector<Tree<tree_leaf>*>>& a, std::pair<std::vector<tree_leaf>, std::vector<Tree<tree_leaf>*>>& b) {
    return a.first.size() < b.first.size();
}

std::vector<std::pair<std::vector<tree_leaf>, std::vector<Tree<tree_leaf>*>>> getCommonClusters(std::vector<Tree<tree_leaf> *> &trees) {
    std::map<std::vector<tree_leaf>, std::vector<Tree<tree_leaf>*>> occurrenceMap;

    for (auto &tree: trees) {
        buildOccurrenceMap(occurrenceMap, tree);
    }
    std::vector<std::pair<std::vector<tree_leaf>, std::vector<Tree<tree_leaf>*>>> commonClusters;
    for (auto &elem: occurrenceMap) {
        if (elem.second.size() == trees.size()) {
            commonClusters.push_back(elem);
        }
    }
    std::sort(commonClusters.begin(), commonClusters.end(), compareSize);

    if (!commonClusters.empty()) {
        // Remove the cluster containing all leafs, because its existence is trivial.
        commonClusters.erase(commonClusters.end());
    }
    return commonClusters;
}


std::vector<std::vector<Tree<tree_leaf>*>> clusterReduction(std::vector<Tree<tree_leaf> *> &trees) {
    auto indexedTrees = indexTrees(trees);
    subtreeReduce(indexedTrees);
    auto commonClusters = getCommonClusters(trees);

    std::vector<std::vector<Tree<tree_leaf>*>> ret;

    for (auto &cluster: commonClusters) {
        std::vector<Tree<tree_leaf>*> newTrees;
        for (auto treeAndClusterTree: boost::combine(indexedTrees, cluster.second)) {
            auto tree = treeAndClusterTree.get<0>();
            auto clusterTree = treeAndClusterTree.get<1>();
            auto parent = tree->parentMap[clusterTree];
            auto representative = new Tree<tree_leaf>();
            representative->name = *cluster.first.begin();
            std::replace(parent->children.begin(), parent->children.end(), clusterTree, representative);
            newTrees.push_back(clusterTree);

        }
        ret.push_back(newTrees);
    }

    ret.push_back(trees);

    for (auto x: indexedTrees) {
        x->tree = NULL;
        delete x;
    }
    return ret;
}



std::vector<short> buildOccurrenceMap(std::map<std::vector<tree_leaf>, std::vector<Tree<tree_leaf>*>> &occurrenceMap, Tree<tree_leaf> *tree) {
    std::vector<short> ret;
    if (tree->children.size() == 0) {
        ret.push_back(tree->name);
    } else if (tree->children.size() == 2) {
        auto res1 = buildOccurrenceMap(occurrenceMap, tree->children[0]);
        auto res2 = buildOccurrenceMap(occurrenceMap, tree->children[1]);
        std::merge(res1.begin(), res1.end(), res2.begin(), res2.end(), std::back_inserter(ret));

        auto mapElem = occurrenceMap.find(ret);
        if (mapElem != occurrenceMap.end()) {
            mapElem->second.push_back(tree);
        }
        else {
            std::vector<Tree<tree_leaf>*> correspondingTrees;
            correspondingTrees.push_back(tree);
            occurrenceMap[ret] = correspondingTrees;
        }
    } else {
        throw std::runtime_error("Wrong child number");
    }
    return ret;
}