#pragma once

#include "tree.h"

bool greedyPick(std::vector<IndexedTree<tree_leaf> *> trees, std::map<tree_leaf, short>* in_all, reco_sequence<tree_leaf>* sequence);
bool greedyPick(std::vector<IndexedTree<tree_leaf>*> trees, std::set<tree_leaf> label, std::map<tree_leaf, short>* in_all, reco_sequence<tree_leaf>* sequence) ;