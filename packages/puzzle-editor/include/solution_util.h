#pragma once

#include "Witness.h"

template <int width, int height>
void DFS(const Witness<width, height>& env, WitnessState<width, height>& state,
         std::vector<WitnessState<width, height>>& solutions) {
    if (env.GoalTest(state)) {
        solutions.push_back(state);
        return;
    }
    auto& actions = *env.actionCache.getItem();
    env.GetActions(state, actions);
    for (const auto& action : actions) {
        env.ApplyAction(state, action);
        DFS(env, state, solutions);
        env.UndoAction(state, action);
    }
    env.actionCache.returnItem(&actions);
}

template <int width, int height>
void GetAllSolutions(const Witness<width, height>& env, WitnessState<width, height>& state,
                     std::vector<WitnessState<width, height>>& puzzles) {
    DFS(env, state, puzzles);
}

template <int width, int height>
auto GetNumSolutions(const Witness<width, height>& puzzle,
                     const std::vector<WitnessState<width, height>>& solutions) {
    return std::accumulate(solutions.begin(), solutions.end(), 0ul,
                           [&puzzle](const std::size_t& sum, const auto& solution) {
                               return sum + static_cast<std::size_t>(puzzle.GoalTest(solution));
                           });
}
