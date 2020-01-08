#pragma once

#include <chrono>
#include "tree.h"
#include "common.h"
enum CherryPickResult { success = 0x000, timeout = 0x100, no_solution = 0x111, no_solution_for_k = 0x110 };

CherryPickResult cherrypickV1(std::vector<IndexedTree<tree_leaf >*> trees, int k);

enum CherryPickAlgorithm { Default, SemiTemporal, NonBinary };

std::tuple<CherryPickResult, int> cherrypickRepeatedly(std::vector<Tree<tree_leaf > *> trees, CherryPickAlgorithm algorithm, int maxTemporalDistance, std::chrono::high_resolution_clock::time_point timeout_time, bool verbose, std::vector<float> &times) ;

std::tuple<CherryPickResult, int, int> cherrypickSemiTemporalRepeatedly(std::vector<Tree<tree_leaf> *> trees, CherryPickAlgorithm algorithm, std::chrono::high_resolution_clock::time_point timeout_time, bool verbose);

void addResult(CherryPickResult &result, CherryPickResult newValue);