#pragma once

#include <iterator>

#include "global.h"

template <int width, int height>
void DFS(const Witness<width, height> &env, WitnessState<width, height> &state,
         std::vector<WitnessState<width, height>> &solutions) {
    if (env.GoalTest(state)) {
        solutions.push_back(state);
        return;
    }
    auto &actions = *env.actionCache.getItem();
    env.GetActions(state, actions);
    for (const auto &action : actions) {
        env.ApplyAction(state, action);
        DFS(env, state, solutions);
        env.UndoAction(state, action);
    }
    env.actionCache.returnItem(&actions);
}

template <int width, int height>
void GetAllSolutions(const Witness<width, height> &env,
                     std::vector<WitnessState<width, height>> &puzzles) {
    WitnessState<width, height> state;
    DFS(env, state, puzzles);
}

template <int width, int height>
auto GetNumSolutions(const Witness<width, height> &witness,
                     std::vector<WitnessState<width, height>> &solutions) {
    return std::accumulate(solutions.begin(), solutions.end(), 0ul,
                           [&](const std::size_t &sum, const auto &solution) {
                               return sum + static_cast<std::size_t>(witness.GoalTest(solution));
                           });
}

inline auto GetCurrentEntropy(const Witness<kPuzzleWidth, kPuzzleHeight> &env) {
    WitnessState<kPuzzleWidth, kPuzzleHeight> state;
    return kEntropy.SetRelative(true).Calculate(env, state, 0, std::nullopt);
}

inline auto GetCurrentAdvEntropy(const Witness<kPuzzleWidth, kPuzzleHeight> &env) {
    WitnessState<kPuzzleWidth, kPuzzleHeight> state;
    return kIWS.ws.path.empty() ? kEntropy.CalculateAdversarialEntropy(env, state, 0)
                                : kEntropy.CalculatePartialAdversarialEntropy(env, kIWS.ws, 0);
}

inline void UpdateEntropy(const Witness<kPuzzleWidth, kPuzzleHeight> &env) {
    kEntropyInfo = GetCurrentEntropy(env);
    kAdvEntropyInfo = GetCurrentAdvEntropy(env);
}

void UpdateSolutionIndices();
