#pragma once
#include <vector>
#include "tree.h"
#include <algorithm>

std::vector<short> buildOccurrenceMap(std::map<std::vector<tree_leaf>, std::vector<Tree<tree_leaf>*>> &occurrenceMap, Tree<tree_leaf> *tree);
std::vector<std::vector<Tree<tree_leaf>*>> clusterReduction(std::vector<Tree<tree_leaf> *> &trees);
