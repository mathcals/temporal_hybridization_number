#pragma once

#include <chrono>
#include "tree.h"
#include "common.h"
#include "cherrypick.h"


/**
 * Cherrypicking algorithm for two non-binary trees.
 * @param trees Two input trees
 * @param k Maximum weight of sequence
 * @param timeout Timeout time
 * @param returnSequence Return sequence
 * @return the result
 */
CherryPickResult cherrypickNonBinaryTrees(std::vector<IndexedTree<tree_leaf>*> trees, int k, std::chrono::high_resolution_clock::time_point timeout, std::vector<std::tuple<tree_leaf, short>>& returnSequence);