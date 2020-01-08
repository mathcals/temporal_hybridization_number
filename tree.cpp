#pragma once
#include <vector>
#include <algorithm>
#include <string>
#include "tree.h"
#include "common.h"
#include <map>
#include <unordered_map>
#include <set>
#include <boost/lexical_cast.hpp>

template <class T>
Tree<T>::Tree(std::vector<Tree<T>*> children, T name) {
        this->children = children;
        this->name = name;
}
template <class T>
Tree<T>::~Tree() {
    for (auto &child : this->children) {
        delete child;
    }
}
template <class T>
Tree<T>::Tree(Tree<T>* tree) {
    this->children = std::vector(tree->children);
    for (auto it = this->children.begin(); it != this->children.end(); ++it)
    {
        (*it) = new Tree(*it);
    }
    this->name = T(tree->name);
}
template <class T>
bool Tree<T> ::isCherry() {
    bool ret = true;
    if (this->children.size() == 0)
        return false;
    for (auto &child : this->children) {
        if(!child->children.empty())
            return false;
    }
    return true;
}


template<> std::string Tree<std::string>::combine() {
        std::string ret = "";
        if (this->children.empty())
            ret += this->name;
        else {
            ret += "(";
            bool first = true;
            std::vector<std::string> sub;
            for (auto &child: this->children) {
                sub.push_back(child->combine());
            }
            std::sort(sub.begin(), sub.end());
            for (std::string &x : sub) {
                if (!first) {
                    ret += ", ";
                } else {
                    first = false;
                }
                ret += x;

            }
            ret += ")";
            ret += this->name;
        }
        return ret;
}

template<> std::set<short> Tree<std::set<short>>::combine() {
    std::set<short> ret;
    std::vector<Tree*> todo;
    todo.push_back(this);
    while (!todo.empty()) {
        auto x = todo[todo.size()-1];
        todo.pop_back();
        ret.insert(x->name.begin(), x->name.end());
        for (auto &child: x->children) {
            ret.insert(child->name.begin(), child->name.end());
            todo.push_back(child);
        }
    }
    return ret;
}


template <> inline std::string defaultValue() {
    return std::string();
}
template <> inline tree_leaf defaultValue() {
    return 0;
}


void subtreeReduce(std::vector<IndexedTree<tree_leaf>*> trees) {
    std::set<tree_leaf> todo;
    for (auto &x: trees[0]->labelMap) {
        todo.insert(x.first);
    }
    subtreeReduce(trees, todo);
}




template <class T>
void updateInAllForChanged(std::vector<IndexedTree<T>*> trees, std::set<T> changed, std::map<T, short>* in_all)  {
    std::set<T> changed_leaves;
    for (auto &x: changed) {
        auto nbrs = neighbours(trees, x);
        changed_leaves.insert(nbrs.begin(), nbrs.end());
    }

    for (auto &x: changed_leaves) {
        std::set<T> neighbors;
        for (auto &tree: trees) {
            auto elem = tree->labelMap[x];
            if (elem == NULL)
                continue;
            auto parent = tree->parentMap[elem];
            if (parent == NULL)
                continue;
            auto ngb = neighbor(parent, elem);
            if (ngb->name == defaultValue<T>()) {
                neighbors.clear();
                break;
            } else {
                neighbors.insert(ngb->name);
            }
        }
        if (in_all != NULL) {
            if (neighbors.size() != 0)
                (*in_all)[x] = neighbors.size();
            else
                in_all->erase(x); //Not needed?
        }
    }
}


void subtreeReduce(std::vector<IndexedTree<tree_leaf>*> trees,std::set<tree_leaf> todo, std::map<tree_leaf, short>* in_all) {
    std::set<tree_leaf> changed;
    for (auto &x : todo) {
        auto elem = subtreeReduce(trees, x, in_all);
        if (in_all != NULL)
            changed.insert(elem);
    }
    if (in_all != NULL) {
        updateInAllForChanged(trees, changed, in_all);
    }
}

void subtreeReduce(std::vector<IndexedTree<tree_leaf>*> trees, std::set<tree_leaf> todo, std::map<tree_leaf, short>* in_all, reco_sequence<tree_leaf>& sequence) {
    std::set<tree_leaf> changed;
    for (auto &x : todo) {
        auto elem = subtreeReduce(trees, x, in_all, sequence);
        if (in_all != NULL)
            changed.insert(elem);
    }
    if (in_all != NULL) {
        updateInAllForChanged(trees, changed, in_all);
    }
}


template <class T>
Tree<T>* neighbor(Tree<T>* parent, Tree<T>* elem) {
    for (auto &x: parent->children) {
        if (x != elem) {
            return x;
        }
    }
    return NULL;
}


std::vector<short> toArray(Tree<short>* cherry) {

    std::vector<short> ret(cherry->children.size());
    int i =0;
    for (auto &child: cherry->children) {
        ret[i]=child->name;
        i++;
    }
    std::sort(ret.begin(), ret.end());
    return ret;
}


template <class T>
std::string Tree<T>::toString() {
    return "";
}
template <>
std::string Tree<short>::toString() {
    if (children.size() == 0) {
        return std::to_string(this->name);
    }
    else {
        std::string ret = "(";
        int i = 0;
        for (auto child: children) {
            if (i !=0) {
                ret += ",";
            }
            ret += child->toString();
            i++;
        }
        return ret + ")";

    }
}



tree_leaf subtreeReduceOneNew(std::vector<IndexedTree<tree_leaf>*> trees, tree_leaf label, std::map<tree_leaf, short>* in_all, reco_sequence<tree_leaf>* sequence) {
    auto nbrs = neighbours(trees, label);
    nbrs.erase(label);
    if (nbrs.size() > 1)
        return 0;
    auto nbr = *nbrs.begin();
    std::set<tree_leaf> ret;
    for (auto &tree :trees) {
        auto elemA = tree->labelMap[label];
        auto elemB = tree->labelMap[nbr];
        if (elemA == NULL && elemB == NULL)
            continue;
        if ((elemA == NULL) != (elemB == NULL))
            return 0;
        if (tree->parentMap[elemA] != tree->parentMap[elemB])
            return 0;
    }
    auto min = std::min(label, nbr);
    auto max = std::max(label, nbr);
    if (in_all != NULL) {
        in_all->erase(max);
    }
    for (auto &tree :trees) {
            tree->remove({max, min}, in_all, sequence);

    }
    return min;
}
short subtreeReduceOne(std::vector<IndexedTree<tree_leaf>*> trees, tree_leaf label, std::map<tree_leaf, short>* in_all) {
    return subtreeReduceOneNew(trees, label, NULL, NULL);
    if (trees[0]->labelMap.count(label) == 0)
        return 0;
    auto elem_zero = trees[0]->labelMap[label];
    if (elem_zero == trees[0]->tree)
        return 0;

    auto par = trees[0]->parentMap[elem_zero];

    if (!par->isCherry())
    {
        return 0;
    }
    auto labels = toArray(par);
    int i = 0;
    bool similar = true;
    for (auto &tree :trees) {
        auto parent = tree->parentMap[ tree->labelMap[label]];
        if (!parent->isCherry())
        {similar = false;
            break;}
        if (i != 0) {
            similar &= toArray(parent) == labels;
            if (!similar) break;
        }
        i++;
    }
    if (similar) {
        for (auto &tree :trees) {
            Tree<short>* parent = tree->parentMap[tree->labelMap[label]];
            if (parent == NULL) {
                throw std::runtime_error("Can not reduce");
            }
            for (auto &child: parent->children) {
                if (child->name != labels[0] && in_all != NULL) {
                        in_all->erase(child->name);
                }
                tree->labelMap.erase(child->name);
                tree->parentMap.erase(child);
                delete child;
            }
            parent->children.clear();
            parent->name = labels[0];
            tree->labelMap[labels[0]] = parent;

        }
        return labels[0];
    }
    return 0;
}

tree_leaf subtreeReduce(std::vector<IndexedTree<tree_leaf>*> trees, tree_leaf item, std::map<tree_leaf, short>* in_all) {
    tree_leaf out = item;
    tree_leaf lastout = defaultValue<tree_leaf>();
    while (out != defaultValue<tree_leaf>()) {
        lastout = out;
        out = subtreeReduceOne(trees, out, in_all);
    }

    return lastout;
}


tree_leaf subtreeReduce(std::vector<IndexedTree<tree_leaf>*> trees, tree_leaf item, std::map<tree_leaf, short>* in_all, reco_sequence<tree_leaf>& sequence) {
    tree_leaf out = item;
    tree_leaf lastout = defaultValue<tree_leaf>();
    while (out != defaultValue<tree_leaf>()) {
        lastout = out;
        out = subtreeReduceOneNew(trees, out, in_all, &sequence);
    }

    return lastout;
}


template <class T>
IndexedTree<T>::IndexedTree(Tree<T>* tree) {
    this->tree = tree;
    this->index(tree);
}

template <class T>
T IndexedTree<T>::remove(T name) {
    T ret = defaultValue<T>();
    auto elem = this->labelMap[name];
    if (elem == NULL) {
        throw std::runtime_error("wrong name");
    }
    auto parent = this->parentMap[elem];
    {
        auto pChildren = &parent->children;
        pChildren->erase(std::remove(pChildren->begin(), pChildren->end(), elem));
    }
    if (parent->children.size() == 1) {
        auto pp = this->parentMap[parent];
        if (pp == NULL) {
            this->tree = parent->children[0];
        }
        else
        {
            auto ppChildren = &pp->children;
            ppChildren->erase(std::remove(ppChildren->begin(), ppChildren->end(), parent));
            auto firstchild = parent->children[0];
            ret = firstchild->name;
            ppChildren->push_back(firstchild);
        }
        this->parentMap[parent->children[0]] = pp;
        parent->children.clear();
        this->parentMap.erase(parent);
        delete parent;
    }
    this->labelMap.erase(name);
    this->parentMap.erase(elem);
    delete elem;
    return ret;
}



template <class T>
IndexedTree<T>::IndexedTree(IndexedTree<T>* tree) : IndexedTree<T>::IndexedTree(new Tree<T>(tree->tree)) {


}
template <class T>
IndexedTree<T>::~IndexedTree() {
    delete this->tree;
}
template <class T>
Tree<T>::Tree() {}


template <class A, class B> Tree<B>* relabel(Tree<A>* tree, std::unordered_map<A,B> map) {
    Tree<B>* ret = new Tree<B>();
    for (auto &child : tree->children) {
        ret->children.push_back(relabel(child, map));
    }
    auto x = map[tree->name];
    if (x != NULL) {
        ret->name = x;
    } else {
        ret->name = 0;
    }
    return ret;
}

template <class T>
std::vector<Tree<tree_leaf> *> labelNumeric(std::vector<Tree<T> *> vector, bool numericString) {
    std::vector<Tree<tree_leaf> *> ret;
    std::unordered_map<T, short> map;
    int newValue = 1;
    IndexedTree indexed(new Tree(vector[0]));

    for (auto &el : indexed.labelMap) {
        if (numericString) {
            newValue = boost::lexical_cast<short>(el.first);
        }
        map[el.first] = newValue;
        newValue++;

    }
    for (Tree<T>* &tree : vector) {
        Tree<tree_leaf>* x =  relabel<T, tree_leaf>(tree, map);

        ret.push_back(x);
    }

    return ret;
}

template std::vector<Tree<tree_leaf> *> labelNumeric(std::vector<Tree<std::string> *> vector, bool stringNumeric);

template <class T>
std::vector<T> IndexedTree<T>::leaves() {
    std::vector<T> ret;
    for (auto &x: this->labelMap) {
        if (x.second != NULL)
            ret.push_back(x.first);
    }
    return ret;
}

template<class T>
T IndexedTree<T>::remove(T name, std::map<T, short>* in_all, reco_sequence<T>* sequence) {
    auto elem = this->labelMap[name];
    if (elem == NULL) {
        return defaultValue<T>();
    }
    auto parent = this->parentMap[elem];
    auto ngb = neighbor(parent, elem);
    Tree<T>* parentparent = this->parentMap[parent];
    int index = 0;
    Tree<T>** tree_position = NULL;
    if (parentparent != NULL) {
        for (auto &x: parentparent->children) {
            if (x == parent) {
                tree_position = &x;
            }
        }
    }
    else {
        tree_position = &this->tree;
    }
    this->parentMap[ngb] = this->parentMap[*tree_position];
    this->parentMap.erase(elem);
    this->parentMap.erase(parent);
    this->labelMap.erase(name);
    *tree_position = ngb;
    if (sequence != NULL) {
        short in_all_value = 0;
        if (in_all != NULL) {
            auto res = in_all->find(name);
            if (res != in_all->end()) {
                in_all_value = res->second;
            }
        }
        ReconstructionItem<T> item = {tree_position, parent,  in_all_value, this, name};
        sequence->push_back(item);
    }
    else {
        parent->children.clear();
        delete parent;
        delete elem;
    }

    return ngb->name;
}


template<class T>
void IndexedTree<T>::remove(ordered_cherry<T> cherry, std::map<T, short>* in_all, reco_sequence<T>* sequence) {
    auto elem = this->labelMap[cherry.first];
    if (elem == NULL) {
        return;
    }
    auto parent = this->parentMap[elem];
    auto ngb = neighbor(parent, elem);
    if (ngb->name != cherry.second)
        return;
    this->remove(cherry.first, in_all, sequence);
}

template<class T>
void IndexedTree<T>::index(Tree<T>* subtree) {
    std::vector<Tree<T>*> todo;
    todo.push_back(subtree);
    while (!todo.empty()) {
        Tree<T>* elem = todo.at(todo.size()-1);
        todo.pop_back();
        this->maxLeaf = std::max(this->maxLeaf, elem->name);
        if (elem->name != defaultValue<T>()) {
            this->labelMap[elem->name]= elem;
        }
        for (Tree<T>* &child : elem->children) {
            todo.push_back(child);
            this->parentMap[child] = elem;
        }
    }
}

template<class T>
Tree<T>* IndexedTree<T>::neighbour(Tree<T>* elem) {
    auto parent=  parentMap[elem];
    if (parent == NULL)
        return NULL;
    for (auto &y: parent->children) {
        if (y != elem) {
            return y;
        }
    }
    throw std::runtime_error("Can not happen");
}

std::string toString(std::vector<Tree<tree_leaf> *> trees) {
    std::string r;
    for (auto &tree: trees) {
        r += tree->toString();
        r += ";\n";
    }
    return r;
}

std::string toString(std::vector<IndexedTree<tree_leaf>*> trees) {
    std::string r;
    for (auto &tree: trees) {
        r += tree->tree->toString();
        r += ";\n";
    }
    return r;
}

template class std::vector<reco_sequence<std::string>>;