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
void GetAllSolutions(const Witness<width, height> &env, WitnessState<width, height> &state,
                     std::vector<WitnessState<width, height>> &puzzles) {
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
    kState = kIWS.ws;
    return kEntropy.SetRelative(true).Calculate(env, kState, 0, std::nullopt);
}

inline auto GetCurrentAdvEntropy(const Witness<kPuzzleWidth, kPuzzleHeight> &env) {
    kState = kIWS.ws;
    return kIWS.ws.path.empty() ? kEntropy.CalculateAdversarialEntropy(env, kState, 0)
                                : kEntropy.CalculatePartialAdversarialEntropy(env, kState, 0);
}

inline void UpdateEntropy(const Witness<kPuzzleWidth, kPuzzleHeight> &env) {
    kEntropyInfo = GetCurrentEntropy(env);
    kAdvEntropyInfo = GetCurrentAdvEntropy(env);
}

inline void UpdateSolutionIndices() {
    kCurrentSolutionIndices.clear();
    for (auto i = 0; i < kAllSolutions.size(); ++i) {
        if (kPuzzle.GoalTest(kAllSolutions[i])) kCurrentSolutionIndices.emplace_back(i);
    }
    if (kSolved) {
        if (kCurrentSolutionIndices.empty())
            kIWS.Reset();
        else {
            kSolutionIndex = 0;
            kIWS.ws = kAllSolutions[kCurrentSolutionIndices[0]];
        }
    }
}

template <int width, int height>
void InferHelper(Witness<width, height> &ref) {  // NOLINT
    if (ref.constraintCount[kUnknownRegionConstraint] == 0) {
        if (GetNumSolutions(ref, kAllSolutions) > 0 &&
            std::find_if(kBest.begin(), kBest.end(), [&ref](const auto &p) {
                return p.SaveToHashString() == ref.SaveToHashString();
            }) == kBest.end()) {
            kBest.emplace_back(ref);
        }
        return;
    }
    ref.CountColors();
    auto addNewColor = ref.colorMap.size() < 2;
    for (auto x = 0; x < width; ++x) {
        for (auto y = 0; y < height; ++y) {
            if (ref.GetRegionConstraint(x, y).type == kUnknownRegionConstraint) {
                for (const auto &[constraint, i, j] : kRegionConstraintItems) {
                    switch (constraint.type) {
                        case kSeparation: {
                            for (const auto &[color, _, _] : kColorItems) {
                                auto containsColor = ref.colorMap.find(color) != ref.colorMap.end();
                                if ((addNewColor && !containsColor) ||
                                    (!addNewColor && containsColor)) {
                                    auto newPuzzle = ref;
                                    newPuzzle.AddSeparationConstraint(x, y, color);
                                    InferHelper(newPuzzle);
                                    break;
                                }
                            }
                            break;
                        }
                        case kStar: {
                            for (const auto &[color, _, _] : kColorItems) {
                                auto containsColor = ref.colorMap.find(color) != ref.colorMap.end();
                                if ((addNewColor && !containsColor) ||
                                    (!addNewColor && containsColor)) {
                                    auto newPuzzle = ref;
                                    newPuzzle.AddStarConstraint(x, y, color);
                                    InferHelper(newPuzzle);
                                    break;
                                }
                            }
                            break;
                        }
                        case kTriangle: {
                            auto newPuzzle = ref;
                            newPuzzle.AddTriangleConstraint(x, y, constraint.parameter);
                            InferHelper(newPuzzle);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        }
    }
}

inline void Infer() {
    kState.Reset();
    kBest.clear();
    kBest.emplace_back(kPuzzle);
    InferHelper(kPuzzle);
    std::sort(kBest.begin() + 1, kBest.end(), [&](auto &a, auto &b) {
        WitnessState<kPuzzleWidth, kPuzzleHeight> sa;
        WitnessState<kPuzzleWidth, kPuzzleHeight> sb;
        return kEntropy.Calculate(a, sa, 0, std::nullopt).value >
               kEntropy.Calculate(b, sb, 0, std::nullopt).value;
    });
}
