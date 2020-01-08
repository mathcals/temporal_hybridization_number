#pragma once
//
// Created by sander on 26-6-19.
//

#include <unordered_set>
#include "tree.h"



std::map<tree_leaf, short> inAllCherries(std::vector<IndexedTree<tree_leaf>*> trees);

template <class T>
std::map<T, T> getInCherries(IndexedTree<T>* tree);


template <class T>
std::set<T> neighbours(std::vector<IndexedTree<T>*> trees, T x);