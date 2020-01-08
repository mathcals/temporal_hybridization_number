#pragma once

#include <chrono>
#include "tree.h"
#include "common.h"
#include "cherrypick.h"
#include "cherrypick_default.h"


CherryPickResult cherrypickSemiTemporal(std::vector<IndexedTree<tree_leaf>*> trees, constraint_set<tree_leaf>& C, int k, int orig_k, int p, std::set<tree_leaf> beforeSet, std::chrono::high_resolution_clock::time_point timeout, std::map<tree_leaf, short>& in_all, std::vector<std::tuple<tree_leaf, short>>& return_sequence);



template <class T>
T defaultValue();

std::set<ordered_cherry<tree_leaf>> getTreeChildPickable(std::vector<IndexedTree<tree_leaf> *> vector, std::map<tree_leaf, short> *pMap);

std::set<ordered_cherry<tree_leaf>> getCherries(std::vector<IndexedTree<tree_leaf> *> &trees, std::set<tree_leaf> &beforeSet);