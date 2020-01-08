//
// Created by sander on 31-07-19.
//

#include <iostream>
#include "util.h"


template<class T>
void assertTree(IndexedTree<T>* x) {
    if (x->tree == NULL)
        throw std::runtime_error("Error during execution");
    auto labelMap = x->labelMap;
    auto parentMap = x->parentMap;
    for (auto &p : x->parentMap) {
        bool found = false;
        if(p.second!= NULL) {
            for (auto &c : p.second->children) {
                if (c == p.first)
                    found = true;
            }
            if (!found)
                throw std::runtime_error("Error during execution");
        }

    }
    x->index(x->tree);
    if (parentMap != x->parentMap)
        throw std::runtime_error("Error during execution");
    if (labelMap != x->labelMap || labelMap.size() != x->labelMap.size())
        throw std::runtime_error("Error during execution");
}

template<class T>
void assertTrees(std::vector<IndexedTree<T>*>& trees) {
    for (auto &x: trees) {
        assertTree(x);
    }
}

template<class T>
void printTrees(std::vector<IndexedTree<T> *> trees) {
    for (auto &tree: trees) {
        std::cout << tree->tree->toString() << std::endl;
    }

    std::cout << "" << std::endl;
}
template<class T>
void printTrees(std::vector<Tree<T> *> trees) {
    for (auto &tree: trees) {
        std::cout << tree->toString() << std::endl;
    }

    std::cout << "" << std::endl;
}

template
void printTrees(std::vector<IndexedTree<tree_leaf> *> trees);
template
void printTrees(std::vector<Tree<tree_leaf> *> trees);
template
void assertTrees(std::vector<IndexedTree<tree_leaf>*>& trees);

std::vector<IndexedTree<tree_leaf>*> indexTrees(std::vector<Tree<tree_leaf>*> trees) {
    std::vector<IndexedTree<tree_leaf>*> ret;
    for (auto x: trees) {
        ret.push_back(new IndexedTree(x));
    }
    return ret;
}



std::set<tree_leaf> leaves(std::vector<IndexedTree<tree_leaf> *> &trees) {
    std::set<tree_leaf> ret;
    for (auto &tree: trees) {
        auto leaves = tree->leaves();
        ret.insert(leaves.begin(), leaves.end());
    }
    return ret;
}
