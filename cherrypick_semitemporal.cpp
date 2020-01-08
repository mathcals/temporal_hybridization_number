#include <iostream>
#include "cherrypick_semitemporal.h"
#include "map"
#include "greedy_pick.h"
#include "util.h"
#include <chrono>

std::map<tree_leaf, short> *
pick(std::vector<IndexedTree<tree_leaf> *> &trees, constraint_set<tree_leaf> &C, int &k, int &orig_k,
     std::map<tree_leaf, short> &in_all, reco_sequence<tree_leaf> &sequence, std::set<tree_leaf> &beforeSet);

bool allTreesFullyReduced(std::vector<IndexedTree<tree_leaf> *> trees) {
    for (auto &tree: trees) {
        if (tree->tree->children.size() != 0) return false;
    }
    return true;
}


/**
 * Implementation of
 * @param trees
 * @param C
 * @param k
 * @param orig_k Value of k to the root call. Called k^\star in the thesis.
 * @param p
 * @param beforeSet Set of all leafs that are not in all trees.
 * @param timeout
 * @param in_all
 * @param return_sequence
 * @return
 */
CherryPickResult cherrypickSemiTemporal(std::vector<IndexedTree<tree_leaf> *> trees, constraint_set<tree_leaf> &C, int k, int orig_k, int p, std::set<tree_leaf> beforeSet, std::chrono::high_resolution_clock::time_point timeout, std::map<tree_leaf, short>& in_all, std::vector<std::tuple<tree_leaf, short>>& return_sequence) {
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
        std::map<tree_leaf, short>* blocked = pick(trees, C, k, orig_k, in_all, sequence, beforeSet);

        if (allTreesFullyReduced(trees)) {
            result = CherryPickResult::success;
            goto bret;
        }
        if (2 * k - ((int) C.C.size()) == 0) {
            result = CherryPickResult::no_solution_for_k;
            goto bret;
        }
        {
            auto candidate_two = branchCandidateCase2(trees, C, blocked, in_all);
            auto candidate_one = branchCandidateCase1(trees, C, blocked, in_all);
            if (std::get<0>(candidate_one) != defaultValue<tree_leaf>()) {
                auto x = std::get<0>(candidate_one);
                auto neigh = std::get<1>(candidate_one);
                constraint_set<tree_leaf> newC(C);
                newC.C.insert({x, neigh});
                std::map<tree_leaf, short> in_all1 = in_all;
                //printTrees(trees);
                addResult(result, cherrypickSemiTemporal(trees, newC, k, orig_k, p, beforeSet, timeout, in_all1,
                                                         return_sequence));
                if (result == CherryPickResult::success) goto bret;
                constraint_set<tree_leaf> newC2(C);
                if (!C.hasKey(neigh)) {
                    newC2.key_count++;
                }
                newC2.C.insert({neigh, x});
                std::map<tree_leaf, short> in_all2 = in_all;
                addResult(result, cherrypickSemiTemporal(trees, newC2, k, orig_k, p, beforeSet, timeout, in_all2,
                                                         return_sequence));
            } else if (std::get<1>(candidate_two) != defaultValue<tree_leaf>()) {
                auto x = std::get<0>(candidate_two);
                auto a = std::get<1>(candidate_two);
                auto b = std::get<2>(candidate_two);
                constraint_set<tree_leaf> newC(C);
                newC.C.insert({x, a});
                newC.C.insert({x, b});
                if (!C.hasKey(x))
                    newC.key_count++;
                std::map<tree_leaf, short> in_all1 = in_all;
                addResult(result, cherrypickSemiTemporal(trees, newC, k, orig_k, p, beforeSet, timeout, in_all1,
                                                         return_sequence));
                if (result == CherryPickResult::success) goto bret;
                constraint_set<tree_leaf> newC2(C);
                newC2.C.insert({a, x});
                if (!C.hasKey(a))
                    newC2.key_count++;
                std::map<tree_leaf, short> in_all2 = in_all;
                addResult(result, cherrypickSemiTemporal(trees, newC2, k, orig_k, p, beforeSet, timeout, in_all2,
                                                         return_sequence));
                if (result == CherryPickResult::success) goto bret;
                constraint_set<tree_leaf> newC3(C);
                newC3.C.insert({b, x});
                if (!C.hasKey(b))
                    newC3.key_count++;
                std::map<tree_leaf, short> in_all3 = in_all;
                addResult(result, cherrypickSemiTemporal(trees, newC3, k, orig_k, p, beforeSet, timeout, in_all3,
                                                         return_sequence));
            }
            else if (p > 0) {
                auto pickable = getTreeChildPickable(trees, blocked);
                auto cherries = getCherries(trees, beforeSet);
                if (cherries.size() > 4 * orig_k) {
                    result = CherryPickResult::no_solution_for_k;
                    goto bret;
                }
                for (auto &cherry: pickable) {
                    assertTrees(trees);
                    auto newBeforeSet = beforeSet;
                    constraint_set<tree_leaf> newC(C);
                    bool removedConstraint = newC.C.erase(cherry) == 1;
                    if ((*blocked)[cherry.first] - (removedConstraint ? 1 : 0) == 1) {
                        auto start = newC.C.lower_bound({cherry.first, 0});
                        auto end = newC.C.lower_bound({cherry.first, SHRT_MAX});
                        newC.C.erase(start, end);
                        newC.key_count--;
                        }
                    else if (removedConstraint) {
                        newC.key_count--;
                    }
                    std::map<tree_leaf, short> in_all1 = in_all;
                    reco_sequence<tree_leaf> tempRecoSequence;
                    for (auto &tree: trees) {
                        tree->remove(cherry, NULL, &tempRecoSequence);
                    }
                    tempRecoSequence.back().weight = 1;
                    auto toPick = neighbours(trees, cherry.first);
                    toPick.merge(neighbours(trees, cherry.second));
                    in_all1 = inAllCherries(trees);
                    greedyPick(trees, toPick, &in_all1, &tempRecoSequence);
                    newBeforeSet.insert(cherry.first);
                    addResult(result,
                              cherrypickSemiTemporal(trees, newC, k - 1, orig_k, p - 1, newBeforeSet, timeout, in_all1,
                                                     return_sequence));
                    reconstruct<tree_leaf>(trees, &in_all1, tempRecoSequence);
                    if (result == CherryPickResult::success) goto bret;

                }
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

/**
 *
 * @param trees
 * @param C
 * @param k
 * @param orig_k
 * @param in_all
 * @param sequence
 * @param beforeSet The set of items that are not in all trees
 * @return
 */
std::map<tree_leaf, short> *
pick(std::vector<IndexedTree<tree_leaf> *> &trees, constraint_set<tree_leaf> &C, int &k, int &orig_k,
     std::map<tree_leaf, short> &in_all, reco_sequence<tree_leaf> &sequence, std::set<tree_leaf> &beforeSet) {
    while (true) {

        short key;
        short value = 0;
        bool first = true;
        auto blocked = new std::map<short, short>();

        for (auto &e: beforeSet) {
            for (auto &n: neighbours(trees, e)) {
                (*blocked)[n] = -1;
            }
        }

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
            if (neighbor_count != (in_all).end() && x.second == (*neighbor_count).second) {
                found = true;
                C.C.erase(C.C.lower_bound({x.first, 0}), C.C.lower_bound({x.first, SHRT_MAX}));
                C.key_count--;
                int weight = (in_all)[x.first] - 1;
                k -= weight;
                orig_k -= weight;
                std::set<tree_leaf> to_reduce;
                for (auto &tree : trees) {
                    to_reduce.insert(tree->remove(x.first, &in_all, &sequence));
                }

                sequence.back().weight = weight;
                in_all.erase(x.first);
                for (auto &x: to_reduce) {
                    to_reduce.merge(neighbours(trees, x));
                }
                updateInAllForChanged(trees, to_reduce, &in_all);
                greedyPick(trees, to_reduce, &in_all, &sequence);

                break;
            }

        }
        if (!found) {
            return blocked;
        }
        delete blocked;

    }
}

std::set<ordered_cherry<tree_leaf>> getTreeChildPickable(std::vector<IndexedTree<tree_leaf> *> trees, std::map<tree_leaf, short> *blocked) {
    std::set<ordered_cherry<tree_leaf>> ret;
    for (auto &tree: trees) {
        for (auto &cherry: getInCherries(tree)) {
             if ((*blocked)[cherry.first] == -1) {
                    continue;
            }
            ret.insert({cherry.first, cherry.second});
        }
    }
    return ret;
}

inline void addResult(CherryPickResult &result, CherryPickResult newValue) {
    result = static_cast<CherryPickResult>(result & newValue);
}

std::set<ordered_cherry<tree_leaf>> getCherries(std::vector<IndexedTree<tree_leaf> *> &trees, std::set<tree_leaf> &beforeSet) {
    std::set<ordered_cherry<tree_leaf>> ret;
    for (auto &tree: trees) {
        for (auto &cherry: getInCherries(tree)) {
            if (beforeSet.find(cherry.second) == beforeSet.end()) {
                ret.insert({std::min(cherry.first, cherry.second), std::max(cherry.first, cherry.second)});
            }

        }
    }
    return ret;
}

template
struct constraint_set<tree_leaf>;

template
class std::set<short>;