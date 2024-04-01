#pragma once

#include "PuzzleEntropy.h"
#include "PuzzleInferenceRule.h"
#include "Witness.h"
#include "util.h"

struct AdversaryEntropyInfo {
    double value;
    unsigned depth;
    bool on_solution_path;
};

template <int width, int height>
class WitnessPuzzleEntropy final : public Entropy<WitnessState<width, height>, WitnessAction> {
    using H = Entropy<WitnessState<width, height>, WitnessAction>;
    vectorCache<AdversaryEntropyInfo> adv_entropy_info_cache_;

public:
    AdversaryEntropyInfo CalculateAdversarialEntropy(
        const SearchEnvironment<WitnessState<width, height>, WitnessAction>& env,
        WitnessState<width, height>& state, const unsigned lookahead) {
        const auto& witness = dynamic_cast<const Witness<width, height>&>(env);
        if (witness.GoalTest(state)) return {0.0, 0, true};
        auto& actions = *H::actCache.getItem();
        witness.GetActions(state, actions);
        if (actions.empty()) {
            H::actCache.returnItem(&actions);
            return {0.0, 0, false};
        }
        H::ruleSet.UpdateActionLogics(witness, state, actions);
        const auto& logics = H::ruleSet.logics;
        if (std::count_if(logics.cbegin(), logics.cend(),
                          [](const auto& logic) { return logic.second == MUST_TAKE; }) > 1 ||
            std::count_if(logics.cbegin(), logics.cend(),
                          [](const auto& logic) { return logic.second == INVALID; }) > 0) {
            H::actCache.returnItem(&actions);
            return {0.0, 0, false};
        }
        auto& children = *adv_entropy_info_cache_.getItem();
        if (auto it =
                std::find_if(actions.cbegin(), actions.cend(),
                             [&](const auto& action) { return logics.at(action) == MUST_TAKE; });
            it != actions.cend()) {
            witness.ApplyAction(state, *it);
            children.emplace_back(CalculateAdversarialEntropy(env, state, lookahead));
            witness.UndoAction(state, *it);
        } else {
            for (it = actions.cbegin(); it != actions.cend();) {
                if (logics.at(*it) == CANNOT_TAKE) {
                    children.emplace_back(AdversaryEntropyInfo{0.0, 0, false});
                    it = actions.erase(it);
                } else
                    ++it;
            }
            for (const auto& action : actions) {
                witness.ApplyAction(state, action);
                children.emplace_back(CalculateAdversarialEntropy(env, state, lookahead));
                witness.UndoAction(state, action);
            }
        }
        H::actCache.returnItem(&actions);
        const bool containsSolutionPath =
            std::find_if(children.cbegin(), children.cend(),
                         [](const auto& info) { return info.on_solution_path; }) != children.cend();
        auto& maxChild = *std::max_element(
            children.begin(), children.end(),
            [&](const auto& info1, const auto& info2) { return info1.value < info2.value; });
        const auto info = AdversaryEntropyInfo{
            maxChild.value +
                ((containsSolutionPath) ? 0 : std::log2(static_cast<double>(children.size()))),
            maxChild.depth + 1, containsSolutionPath};
        adv_entropy_info_cache_.returnItem(&children);
        return info;
    }

    AdversaryEntropyInfo CalculatePartialAdversarialEntropy(
        const SearchEnvironment<WitnessState<width, height>, WitnessAction>& env,
        WitnessState<width, height>& state, const unsigned lookahead) {
        if (state.path.size() <= 1) return CalculateAdversarialEntropy(env, state, lookahead);
        const auto& witness = dynamic_cast<const Witness<width, height>&>(env);
        if (witness.GoalTest(state)) return {0.0, 0};
        auto s = WitnessState<width, height>();
        witness.ApplyAction(s, kStart);
        auto [x0, y0] = s.path[0];
        auto& actions = *H::actCache.getItem();
        for (auto i = 1; i < state.path.size(); ++i) {
            witness.GetActions(s, actions);
            H::ruleSet.FilterActions(witness, s, actions);
            auto [x1, y1] = state.path[i];
            assert(!(x1 == x0 + 1 && y1 == y0 + 1));
            WitnessAction action{};
            if (x1 == x0 + 1) action = kRight;
            if (x1 == x0 - 1) action = kLeft;
            if (y1 == y0 + 1) action = kUp;
            if (y1 == y0 - 1) action = kDown;
            if (contains(witness.goal, state.path[i])) action = kEnd;
            if (contains(actions, action)) {
                H::actCache.returnItem(&actions);
                return {0.0, 0, false};
            }
            witness.ApplyAction(s, action);
            x0 = x1;
            y0 = y1;
        }
        H::actCache.returnItem(&actions);
        return CalculateAdversarialEntropy(env, state, lookahead);
    }

    double CalculateConditionalSolutionInformation(const Witness<width, height>& env,
                                                   WitnessState<width, height>& state) {
        if (env.GoalTest(state)) return 0.0;
        auto& actions = *H::actCache.getItem();
        env.GetActions(state, actions);
        H::ruleSet.FilterActions(env, state, actions);
        if (actions.empty()) {
            H::actCache.returnItem(&actions);
            return H::inf;
        }
        auto& children = *H::doubleCache.getItem();
        for (const auto& action : actions) {
            env.ApplyAction(state, action);
            children.emplace_back(CalculateConditionalSolutionInformation(env, state));
            env.UndoAction(state, action);
        }
        H::actCache.returnItem(&actions);
        double information =
            std::log2(children.size() / std::accumulate(children.cbegin(), children.cend(), 0.0,
                                                        [](const auto& sum, const auto& info) {
                                                            return sum + std::exp2(-info);
                                                        }));
        H::doubleCache.returnItem(&children);
        return information;
    }
};
