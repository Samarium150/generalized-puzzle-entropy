#pragma once

#include "PuzzleInferenceRule.h"
#include "Witness.h"
#include "util.h"

struct SolutionTreeNode {
    WitnessAction action = kStart;
    std::array<int, kWitnessActionCount> children{};

    explicit SolutionTreeNode(const WitnessAction action) : action(action) { children.fill(-1); }
};

extern std::vector<SolutionTreeNode> kSolutionTree;

inline int InsertNode(std::vector<SolutionTreeNode>& tree, const int parent_index,
                      WitnessAction action) {
    const auto index = static_cast<int>(tree.size());
    tree.emplace_back(action);
    if (parent_index != -1) tree[parent_index].children[static_cast<std::size_t>(action)] = index;
    return index;
}

[[maybe_unused]] inline void PrintTree(const std::vector<SolutionTreeNode>& tree) {
    for (auto i = 0; i < tree.size(); ++i) {
        std::cout << "Node " << i << ": Action = " << tree[i].action << ", Children = ";
        for (const auto childIndex : tree[i].children) {
            if (childIndex != -1) std::cout << childIndex << " ";
        }
        std::cout << std::endl;
    }
}

template <int width, int height>
void BuildTree(const Witness<width, height>& puzzle,
               const std::vector<WitnessState<width, height>>& solutions,
               std::vector<SolutionTreeNode>& tree) {
    tree.clear();
    tree.reserve(std::numeric_limits<uint16_t>::max());
    Witness<width, height> w;
    w.pathConstraints = puzzle.pathConstraints;
    //  std::for_each(w.pathConstraints.begin(), w.pathConstraints.end(),
    //                [](auto &constraint) {
    //                    if (constraint == kMustCross)
    //                        constraint = kNoPathConstraint;
    //                });
    auto& actions = *w.actionCache.getItem();
    InsertNode(tree, -1, kStart);
    for (const auto& solution : solutions) {
        if (!w.GoalTest(solution)) continue;
        int index = 0;
        w.GetActionSequence(solution, actions);
        for (auto i = 1; i < actions.size(); ++i) {
            const auto& action = actions[i];
            if (const int child = tree[index].children[static_cast<unsigned>(action)]; child == -1)
                index = InsertNode(tree, index, action);
            else
                index = child;
        }
        actions.clear();
    }
    w.actionCache.returnItem(&actions);
    tree.shrink_to_fit();
}

static bool CompareSC(const WitnessRegionConstraint& a, const WitnessRegionConstraint& b) {
    return a.type == kSeparation && b.type == kSeparation && a != b;
}

template <int width, int height>
ActionType SeparationRule(const SearchEnvironment<WitnessState<width, height>, WitnessAction>& env,
                          const WitnessState<width, height>& state, const WitnessAction& action) {
    if (state.path.empty()) return UNKNOWN;
    const auto& witness = static_cast<const Witness<width, height>&>(env);
    const auto& [currX, currY] = state.path.back();
    switch (action) {
        case kUp: {
            if (currX > 0 && currX < width &&
                CompareSC(witness.GetRegionConstraint(currX - 1, currY),
                          witness.GetRegionConstraint(currX, currY)))
                return MUST_TAKE;
            break;
        }
        case kRight: {
            if (currY > 0 && currY < height &&
                CompareSC(witness.GetRegionConstraint(currX, currY),
                          witness.GetRegionConstraint(currX, currY - 1)))
                return MUST_TAKE;
            break;
        }
        case kDown: {
            if (currX > 0 && currX < width &&
                CompareSC(witness.GetRegionConstraint(currX - 1, currY - 1),
                          witness.GetRegionConstraint(currX, currY - 1)))
                return MUST_TAKE;
            break;
        }
        case kLeft: {
            if (currY > 0 && currY < height &&
                CompareSC(witness.GetRegionConstraint(currX - 1, currY - 1),
                          witness.GetRegionConstraint(currX - 1, currY)))
                return MUST_TAKE;
            break;
        }
        default:
            break;
    }
    if (witness.constraintCount[kStar] > 0) {
        witness.GetLeftRightRegions(state);
        for (const auto& [color, regions] : witness.colorMap) {
            if (regions.size() == 2) {
                int r1 = regions[0];
                int r2 = regions[1];
                if (auto &rc1 = witness.GetRegionConstraint(r1),
                    &rc2 = witness.GetRegionConstraint(r2);
                    (rc1.type == kStar || rc2.type == kStar) &&
                    ((contains(state.lhs, r1) && contains(state.rhs, r2)) ||
                     (contains(state.lhs, r2) && contains(state.rhs, r1))))
                    return CANNOT_TAKE;
            }
        }
    }
    return UNKNOWN;
}

template <int width, int height>
ActionType PathConstraintRule(
    const SearchEnvironment<WitnessState<width, height>, WitnessAction>& env,
    const WitnessState<width, height>& state, const WitnessAction& action) {
    if (state.path.empty()) return UNKNOWN;
    const auto& witness = static_cast<const Witness<width, height>&>(env);
    const auto& [currX, currY] = state.path.back();
    switch (action) {
        case kUp: {
            if (witness.GetMustCrossConstraint(false, currX, currY)) return MUST_TAKE;
            if (witness.GetCannotCrossConstraint(false, currX, currY)) return CANNOT_TAKE;
            break;
        }
        case kRight: {
            if (witness.GetMustCrossConstraint(true, currX, currY)) return MUST_TAKE;
            if (witness.GetCannotCrossConstraint(true, currX, currY)) return CANNOT_TAKE;
            break;
        }
        case kDown: {
            if (witness.GetMustCrossConstraint(false, currX, currY - 1)) return MUST_TAKE;
            if (witness.GetCannotCrossConstraint(false, currX, currY - 1)) return CANNOT_TAKE;
            break;
        }
        case kLeft: {
            if (witness.GetMustCrossConstraint(true, currX - 1, currY)) return MUST_TAKE;
            if (witness.GetCannotCrossConstraint(true, currX - 1, currY)) return CANNOT_TAKE;
            break;
        }
        default:
            break;
    }
    return UNKNOWN;
}

template <int width, int height>
ActionType InsideSolutionTreeRule(
    const SearchEnvironment<WitnessState<width, height>, WitnessAction>& env,
    const WitnessState<width, height>& state, const WitnessAction& action) {
    if (action == kStart || action == kEnd) return MUST_TAKE;
    if (kSolutionTree.empty()) return UNKNOWN;
    const auto& witness = static_cast<const Witness<width, height>&>(env);
    auto& actions = *witness.actionCache.getItem();
    actions.clear();
    witness.GetActionSequence(state, actions);
    actions.emplace_back(action);
    int index = 0;
    for (auto i = 1; i < actions.size(); ++i) {
        index = kSolutionTree[index].children[static_cast<unsigned>(actions[i])];
        if (index == -1) {
            witness.actionCache.returnItem(&actions);
            return CANNOT_TAKE;
        }
    }
    witness.actionCache.returnItem(&actions);
    return UNKNOWN;
}

template <int width, int height>
ActionType RegionCompletionRule(
    const SearchEnvironment<WitnessState<width, height>, WitnessAction>& env,
    WitnessState<width, height>& state, const WitnessAction& action) __attribute__((optnone)) {
    if (state.path.empty()) return UNKNOWN;
    const auto& witness = static_cast<const Witness<width, height>&>(env);
    witness.ApplyAction(state, action);
    bool regionSatisfied = true;
    if (const auto& head = state.path.back();
        (state.HitTheWall() && !state.IsAlongTheWall()) ||
        (contains(witness.goal, head) && witness.goalMap[witness.GetPathIndex(head)] != 0))
        regionSatisfied = witness.RegionTest(state);
    witness.UndoAction(state, action);
    return regionSatisfied ? UNKNOWN : CANNOT_TAKE;
}

template <int width, int height>
ActionType AlongThePathRule(
    const SearchEnvironment<WitnessState<width, height>, WitnessAction>& env,
    WitnessState<width, height>& state, const WitnessAction& action) {
    if (state.path.empty()) return UNKNOWN;
    const auto& witness = static_cast<const Witness<width, height>&>(env);
    witness.ApplyAction(state, action);
    const bool satisfied = witness.PathTest(state);
    witness.UndoAction(state, action);
    return satisfied ? UNKNOWN : CANNOT_TAKE;
}

enum WitnessInferenceRule {
    kSeparationRule,
    kPathConstraintRule,
    kInsideSolutionTreeRule,
    kRegionCompletionRule,
    kAlongThePathRule,
    kInferenceRuleCount [[maybe_unused]]
};

inline std::ostream& operator<<(std::ostream& os, const WitnessInferenceRule rule) {
    switch (rule) {
        case kSeparationRule:
            return os << "SeparationRule";
        case kPathConstraintRule:
            return os << "PathConstraintRule";
        case kInsideSolutionTreeRule:
            return os << "InsideSolutionTreeRule";
        case kRegionCompletionRule:
            return os << "RegionCompletionRule";
        case kAlongThePathRule:
            return os << "AlongThePathRule";
        default:
            return os;
    }
}

template <int width, int height>
std::unordered_map<int, std::function<ActionType(
                            const SearchEnvironment<WitnessState<width, height>, WitnessAction>&,
                            WitnessState<width, height>&, const WitnessAction&)>>
    kWitnessInferenceRules = {
        {kSeparationRule, SeparationRule<width, height>},
        {kPathConstraintRule, PathConstraintRule<width, height>},
        {kInsideSolutionTreeRule, InsideSolutionTreeRule<width, height>},
        {kRegionCompletionRule, RegionCompletionRule<width, height>},
        {kAlongThePathRule, AlongThePathRule<width, height>},
};
