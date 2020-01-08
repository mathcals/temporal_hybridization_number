#pragma once
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <boost/unordered_set.hpp>

#include <fstream>
#include <sstream>
#include <queue>


using tree_leaf = short;
template <class T> using ordered_cherry = std::pair<T,T>;

/**
 * Tree data structure
 * @tparam T Type of leaf labels
 */
template <class T>
class Tree {
public:
    std::vector<Tree*> children;
    T name;
    explicit Tree();
    explicit Tree(Tree* tree);
    Tree(std::vector<Tree*> children, T name);
    std::string toString();
    inline T combine();
    bool isCherry();
    ~Tree();

};

template <class T> class IndexedTree;

/**
 * ReconstructionItem Allows for reconstructing a previous version of the trees, by placing element at position target and
 * setting in_all[elem_name]=in_all_value.
 * @tparam T
 */
template <class T>
struct ReconstructionItem {
    Tree<T>** target;
    Tree<T>* elem;
    short in_all_value;
    IndexedTree<T>* parentTree;
    T elem_name;
    short weight;
};


template <class T>
using reco_sequence = std::vector<ReconstructionItem<T>>;
template <class T>
class IndexedTree {
public:
    IndexedTree(IndexedTree<T>* tree);
    explicit IndexedTree(Tree<T>* tree);
    ~IndexedTree();
    Tree<T>* tree;
    std::unordered_map<T,  Tree<T>*> labelMap;
    std::unordered_map<Tree<T>*,  Tree<T>*> parentMap;
    T remove(T name);
    void remove(ordered_cherry<T> cherry, std::map<T, short>* in_all,  reco_sequence<T>* sequence);
    T remove(T name, std::map<T, short>* in_all,  reco_sequence<T>* sequence);
    Tree<T>* neighbour(Tree<T>* elem);
    void index(Tree<T>* subtree);
    std::vector<T> leaves();
    T maxLeaf;
};



void subtreeReduce(std::vector<IndexedTree<tree_leaf>*> trees);

tree_leaf subtreeReduce(std::vector<IndexedTree<tree_leaf>*> trees, tree_leaf item, std::map<tree_leaf, short>* = NULL);


void subtreeReduce(std::vector<IndexedTree<tree_leaf>*> trees, std::set<tree_leaf> item, std::map<tree_leaf, short>* = NULL);



tree_leaf subtreeReduce(std::vector<IndexedTree<tree_leaf>*> trees, tree_leaf item, std::map<tree_leaf, short> *in_all, reco_sequence<tree_leaf> &sequence);


void subtreeReduce(std::vector<IndexedTree<tree_leaf>*> trees, std::set<tree_leaf> todo, std::map<tree_leaf, short> *in_all, reco_sequence<tree_leaf> &sequence);

template <class T> std::vector<Tree<tree_leaf> *> labelNumeric(std::vector<Tree<T> *> vector);

template class Tree<std::string>;
template class IndexedTree<std::string>;
template class Tree<tree_leaf>;
template class IndexedTree<tree_leaf>;
template <class T> std::vector<Tree<tree_leaf> *> labelNumeric(std::vector<Tree<T> *> vector, bool stringNumeric = false);

template <class T>
inline T defaultValue();

std::string toString(std::vector<IndexedTree<tree_leaf> *> trees);

std::string toString(std::vector<IndexedTree<tree_leaf>*> trees);

template <class T>
void updateInAllForChanged(std::vector<IndexedTree<T>*> trees, std::set<T> changed, std::map<T, short>* in_all);