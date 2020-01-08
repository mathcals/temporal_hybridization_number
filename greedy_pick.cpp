//
// Created by sander on 16-08-19.
//

#include "greedy_pick.h"
#include "tree.h"
#include "common.h"

tree_leaf greedyPickOne(std::vector<IndexedTree<tree_leaf> *> trees, const short label, std::map<tree_leaf, short>* in_all, reco_sequence<tree_leaf> *sequence) {
    tree_leaf neighbourLabel = defaultValue<tree_leaf>();
    for (auto &tree: trees) {
        auto elem = tree->labelMap[label];
        if (elem == NULL)
            continue;
        if (elem == tree->tree) {
            neighbourLabel = defaultValue<tree_leaf>();
            break;
        }
        auto ngb = tree->neighbour(elem);
        if (ngb->name == defaultValue<tree_leaf>()) {
            neighbourLabel = defaultValue<tree_leaf>();
            break;
        }
        else if (neighbourLabel == defaultValue<tree_leaf >()) {
            neighbourLabel = ngb->name;
        }
        else if (ngb->name != neighbourLabel) {
            neighbourLabel = defaultValue<tree_leaf>();
            break;
        }

    }
    if (neighbourLabel == defaultValue<tree_leaf>())
        return 0;
    for (auto &tree: trees) {
        tree->remove(label, in_all, sequence);
    }
    if (in_all != NULL)
        in_all->erase(label);
    return neighbourLabel;
}

bool greedyPick(std::vector<IndexedTree<tree_leaf>*> trees, std::set<tree_leaf> todo, std::map<tree_leaf, short>* in_all, reco_sequence<tree_leaf>* sequence) {
    bool success = false;
    std::set<tree_leaf> changed;
    while (!todo.empty()) {
        tree_leaf new_todo = greedyPickOne(trees, *todo.begin(), in_all, sequence);
        todo.erase(todo.begin());
        if (new_todo != defaultValue<tree_leaf>()) {
            changed.merge(neighbours(trees, new_todo));
            todo.merge(neighbours(trees, new_todo));
            success = true;
        }
    }
    updateInAllForChanged(trees, changed, in_all);
    return success;
}
bool greedyPick(std::vector<IndexedTree<tree_leaf>*> trees, std::map<tree_leaf, short>* in_all, reco_sequence<tree_leaf>* sequence) {
    bool changed = true;
    changed = false;
    std::set<tree_leaf> allElements;
    for (auto &tree: trees) {
        for (auto &x: tree->labelMap) {
            allElements.insert(x.first);
        }
    }
    return greedyPick(trees, allElements, in_all, sequence);


}
