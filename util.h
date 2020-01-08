#pragma once

#include "tree.h"

template <class T>
void assertTree(IndexedTree<T>* x);
template <class T>
void assertTrees(std::vector<IndexedTree<T>*>& trees);
template <class T>
void printTrees(std::vector<IndexedTree<T> *> trees) ;
template <class T>
void printTrees(std::vector<Tree<T> *> trees) ;

std::vector<IndexedTree<tree_leaf>*> indexTrees(std::vector<Tree<tree_leaf>*> trees);

std::set<tree_leaf> leaves(std::vector<IndexedTree<tree_leaf> *> &trees);
