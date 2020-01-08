//
// Created by sander on 26-6-19.
//

#include "common.h"
#include "util.h"

template <class T>
std::map<T, T> getInCherries(IndexedTree<T>* tree) {
    std::map<T,T> ret;
    for (auto &x: tree->labelMap) {
        if(x.second == tree->tree || x.second == NULL)
            continue;
        Tree<T>* parent = tree->parentMap[x.second];
        if (parent != NULL) {
            for (auto &child: parent->children) {
                if (child->name != x.first && child->children.empty())
                    ret[x.first] = child->name;
            }
        }
    }
    return ret;
}



std::map<tree_leaf, short> inAllCherries(std::vector<IndexedTree<tree_leaf>*> trees) {
    std::map<tree_leaf, std::set<tree_leaf>> pre_ret;
    for (auto &el: leaves(trees)) {
        std::set<tree_leaf> set;
        pre_ret[el] = set;
    }
    auto tree = trees.begin();
    while (tree != trees.end()) {
        auto it = pre_ret.begin();
        while (it != pre_ret.end()) {
            auto elem = (*tree)->labelMap[(*it).first];
            if (elem == NULL) {
                it++;
                continue;
            }
            auto parent = (*tree)->parentMap[elem];
            bool found_neighbour = false;
            if (parent != NULL) {
                for (auto &child: parent->children) {
                    if (child->name != (*it).first && child->children.empty()) {
                        pre_ret[(*it).first].insert(child->name);
                        found_neighbour = true;
                    }
                }
            }

            if (!found_neighbour) {

                it = pre_ret.erase(it);
            }
            else {
                ++it;
            }
        }
        tree++;
    }

    std::map<tree_leaf, short> ret;
    for (auto &el: pre_ret) {
        ret[el.first] = (short)el.second.size();
    }
    return ret;
}

template <class T>
std::set<T> neighbours(std::vector<IndexedTree<T>*> trees, T x) {
    std::set<T> ret;
    ret.insert(x);
    for (auto &tree: trees) {
        auto elem = tree->labelMap[x];
        if (elem == NULL)
            continue;
        auto neighbour = tree->neighbour(elem);
        if (neighbour != NULL && neighbour->name != defaultValue<T>())
        ret.insert(neighbour->name);
    }
    return ret;
}


template std::map<std::string, std::string> getInCherries(IndexedTree<std::string>* tree);



template class std::map<std::string, short>;
template std::set<std::string> neighbours(std::vector<IndexedTree<std::string>*> trees, std::string x);
template std::set<short> neighbours(std::vector<IndexedTree<short>*> trees, short x);
template std::map<tree_leaf, tree_leaf> getInCherries(IndexedTree<tree_leaf>* tree);