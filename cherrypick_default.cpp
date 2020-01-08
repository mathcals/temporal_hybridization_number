#include <iostream>
#include "cherrypick_default.h"
#include "map"
#include "util.h"
#include <chrono>

std::map<tree_leaf, short>* pick(std::vector<IndexedTree<tree_leaf> *> &trees, constraint_set<tree_leaf> &C, int& k, std::map<tree_leaf, short>& in_all, reco_sequence<tree_leaf>& sequence) {
    while (true) {

        short key;
        short value = 0;
        bool first = true;
        auto blocked = new std::map<short, short>();
        for (auto it = C.C.begin(); true; it++) {
            if (first && it == C.C.end()) {
                break;
            }
            else if (!first && (it == C.C.end() || key != (*it).first)) {
                if ((*blocked)[key] != -1)
                    (*blocked)[key] = value;
                if (it == C.C.end())
                    break;
                value = 0;
            } else if (first) {
                first = false;
            }
            if ((*it).second!=0) {
                value++;
                (*blocked)[(*it).second] = -1;
            }
            key = (*it).first;
        }
        bool found = false;

        for (auto &x: *blocked) {
            auto neighbor_count = (in_all).find(x.first);
            if (neighbor_count != (in_all).end() && x.second == (*neighbor_count).second  && x.second != 0) {
                found = true;
                C.C.erase(C.C.lower_bound({x.first, 0}), C.C.lower_bound({x.first, SHRT_MAX})); //TODO make more efficient
                C.key_count--;
                int weight = (in_all)[x.first] - 1;
                k -= weight;
                std::set<short> to_reduce;

                for (auto &tree : trees) {
                    to_reduce.insert(tree->remove(x.first, &in_all, &sequence));
                }
                sequence.back().weight = weight;

                for (auto &x: to_reduce) {
                    to_reduce.merge(neighbours(trees, x));
                }
                updateInAllForChanged(trees, to_reduce, &in_all);
                subtreeReduce(trees, to_reduce, &in_all, sequence);
                in_all.erase(x.first);
                auto in_all2 = inAllCherries(trees);
                auto it = in_all.cbegin();
                while (it != in_all.cend()) {
                    if((*it).second == 0 && false) {
                        in_all.erase(it++);
                    } else {
                        it++;
                    }
                }
                if (in_all != in_all2) {
                //    throw "x";
                }

                break;
            }

        }
        if (!found) {
            return blocked;
        }
        delete blocked;

    }
}



std::tuple<tree_leaf, tree_leaf>
branchCandidateCase1(std::vector<IndexedTree<tree_leaf> *> &trees, constraint_set<tree_leaf> &C, std::map<tree_leaf, short>* blocked, std::map<tree_leaf, short>& in_all) {
    auto keys = C.keys();
    for (auto &x: keys) {
        for (auto &neigh : neighbours(trees, x)) {
            if (x != neigh && C.C.find({x, neigh}) == C.C.end() && C.C.find({neigh, x}) == C.C.end()) {
                return {x, neigh};
            }
        }

    }
    return {0, 0};
}

std::tuple<tree_leaf, tree_leaf, tree_leaf>
branchCandidateCase2(std::vector<IndexedTree<tree_leaf> *> &trees, constraint_set<tree_leaf> &C, std::map<tree_leaf, short>* blocked, std::map<tree_leaf, short>& in_all) {
    for (auto &x: in_all) {
        if (x.second == 0)
            continue;
        auto bl = (*blocked)[x.first];
        if (bl != -1) {
            if (bl == 0) {
                auto nbrs = neighbours(trees, x.first);
                nbrs.erase(x.first);
                auto it = nbrs.begin();
                auto a = *it;
                if (it == nbrs.end()) {
                    printTrees(trees);
                    throw std::runtime_error("Error during execution");
                }
                it++;
                auto b = *it;
                return {x.first, a, b};
            }
            else {
                return {x.first, 0, 0};
            }
        }

    }
    return {0, 0, 0};
}

template<class T>
void reconstruct(std::vector<IndexedTree<T> *> &trees, std::map<T, short>* in_all, const reco_sequence<T> &sequence) {
    for (auto it = sequence.rbegin(); it != sequence.rend(); ++it)
    {
        Tree<T>** target = it->target;
        Tree<T>* elem = it->elem;
        auto ntree = it->parentTree;
        auto parent = ntree->parentMap[*target];
        ntree->parentMap[elem] = parent;
        *target = elem;
        if (in_all != NULL && elem->name != defaultValue<T>() && it->in_all_value != 0)
            (*in_all)[elem->name] = it->in_all_value;
        ntree->index(elem);

    }
}

int call = 0;
CherryPickResult cherrypickDefault(std::vector<IndexedTree<tree_leaf> *> trees, constraint_set<tree_leaf> &C, int k, std::chrono::high_resolution_clock::time_point timeout, std::map<tree_leaf, short>& in_all, std::vector<std::tuple<tree_leaf, short>>& return_sequence) {
    int tcall = call;
    call++;
    CherryPickResult result = CherryPickResult::no_solution;
    if (C.C.size() == 0 && C.key_count > 0) {
        throw std::runtime_error("Error during execution");
    }
    if (std::chrono::high_resolution_clock::now() > timeout) {
        result = CherryPickResult::timeout;
        goto ret;
    }

    {
        int one_count = 0;
        tree_leaf last;
        int count = -1;
        for (auto &c: C.C) {
            if (count == -1) {
                last = c.first;
                count = 1;
            } else if (last == c.first) {
                count++;
            } else {
                if (count == 1) {
                    one_count++;
                }
                last = c.first;
                count = 1;
            }
        }
        if (count == 1) {
            one_count++;
        }

        if (2 * k - ((int) C.C.size()) - one_count < 0) {
            result = CherryPickResult::no_solution_for_k;
            goto ret;
        }
    }
    {

        reco_sequence<tree_leaf> sequence;
        std::map<tree_leaf, short>* blocked = pick(trees, C, k, in_all, sequence);
        for (auto item : sequence) {
            auto v = (*blocked)[item.elem_name];
            if (v != 0) {
                result = CherryPickResult::no_solution;
                goto bret;
            }
        }

        if (trees[0]->tree->children.empty()) {
            result = CherryPickResult::success;
            goto bret;
        }
        if (2 * k - ((int) C.C.size()) == 0) {
            result = CherryPickResult::no_solution_for_k;
            goto bret;
        }
        {

            if (in_all.size() > 3 * k) {
                result = CherryPickResult::no_solution_for_k;
                goto bret;
            } else if (in_all.empty()) {
                result = CherryPickResult::no_solution;
                goto bret;
            }

            auto candidate_two = branchCandidateCase2(trees, C, blocked, in_all);
            if (std::get<0>(candidate_two) == defaultValue<tree_leaf>()) {
                goto bret;
            }

            auto candidate_one = branchCandidateCase1(trees, C, blocked, in_all);
            if (std::get<0>(candidate_one) != defaultValue<tree_leaf>()) {
                auto x = std::get<0>(candidate_one);
                auto neigh = std::get<1>(candidate_one);
                constraint_set<tree_leaf> newC(C);
                newC.C.insert({x, neigh});
                std::map<tree_leaf, short> in_all1 = in_all;
                addResult(result, cherrypickDefault(trees, newC, k, timeout, in_all1, return_sequence));
                if (result == CherryPickResult::success) goto bret;
                constraint_set<tree_leaf> newC2(C);
                bool is_key = false;
                for (auto &x: C.C) {
                    if (x.first == neigh) {
                        is_key = true;
                    }
                }
                if (!is_key) {
                    newC2.key_count++;
                }
                newC2.C.insert({neigh, x});
                addResult(result, cherrypickDefault(trees, newC2, k, timeout, in_all, return_sequence));
            } else {
                auto x = std::get<0>(candidate_two);
                auto a = std::get<1>(candidate_two);
                auto b = std::get<2>(candidate_two);
                constraint_set<tree_leaf> newC(C);
                newC.C.insert({x, a});
                newC.C.insert({x, b});
                newC.key_count++;
                std::map<tree_leaf, short> in_all1 = in_all;
                addResult(result, cherrypickDefault(trees, newC, k, timeout, in_all1, return_sequence));
                if (result == CherryPickResult::success) goto bret;
                constraint_set<tree_leaf> newC2(C);
                newC2.C.insert({a, x});
                newC2.key_count++;
                std::map<tree_leaf, short> in_all2 = in_all;
                addResult(result, cherrypickDefault(trees, newC2, k, timeout, in_all2, return_sequence));
                if (result == CherryPickResult::success) goto bret;
                constraint_set<tree_leaf> newC3(C);
                newC3.C.insert({b, x});
                newC3.key_count++;
                addResult(result, cherrypickDefault(trees, newC3, k, timeout, in_all, return_sequence));
            }
        }
        bret:
        delete blocked;
        if (result == CherryPickResult::success) {
            for (auto pos = sequence.rbegin(); pos != sequence.rend(); pos++) {
                if (pos->weight != 0)
                return_sequence.insert(return_sequence.begin(), {pos->elem_name, pos->weight});
            }

        }
        reconstruct<tree_leaf>(trees, &in_all, sequence);


    }
    ret:
    return result;

}

inline void addResult(CherryPickResult &result, CherryPickResult newValue) {
    result = static_cast<CherryPickResult>(result & newValue);
}



template
struct constraint_set<tree_leaf>;

template
class std::set<short>;