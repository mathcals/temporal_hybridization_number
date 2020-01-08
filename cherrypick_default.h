#pragma once

#include <chrono>
#include "tree.h"
#include "common.h"
#include "cherrypick.h"

template <class T>
class constraint_set{
public :
    std::set<std::pair<T,T>> C;
    int key_count =0;

    bool hasKey(T key) {
        for (auto &x: this->C) {
            if (x.first == key) {
                return true;
            }
        }
        return false;
    }

    std::set<T> keys() {
        std::set<T> ret;
        const T* last = NULL;
        for (auto &x: this->C) {
            if (last==NULL || *last != x.first) {
                ret.insert(x.first);
                last = &x.first;
            }
        }
        return ret;
    }
};


CherryPickResult cherrypickDefault(std::vector<IndexedTree<tree_leaf>*> trees, constraint_set<tree_leaf>& C, int k, std::chrono::high_resolution_clock::time_point timeout, std::map<tree_leaf, short>& in_all, std::vector<std::tuple<tree_leaf, short>>& return_sequence);

std::tuple<tree_leaf, tree_leaf>
branchCandidateCase1(std::vector<IndexedTree<tree_leaf> *> &trees, constraint_set<tree_leaf> &C, std::map<tree_leaf, short>* blocked, std::map<tree_leaf, short>& in_all);

std::tuple<tree_leaf, tree_leaf, tree_leaf>
branchCandidateCase2(std::vector<IndexedTree<tree_leaf> *> &trees, constraint_set<tree_leaf> &C, std::map<tree_leaf, short>* blocked, std::map<tree_leaf, short>& in_all);

template <class T>
T defaultValue();

inline void addResult(CherryPickResult &result, CherryPickResult newValue);

template <class T>
void reconstruct(std::vector<IndexedTree<T> *> &trees, std::map<T, short>* in_all, const reco_sequence<T> &sequence);