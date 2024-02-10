#pragma once

#include "global.h"

inline auto GetCurrentEntropy(const Witness<kPuzzleWidth, kPuzzleHeight>& env) {
    kState = kIWS.ws;
    return kEntropy.SetRelative(true).Calculate(env, kState, 0, std::nullopt);
}

inline auto GetCurrentAdvEntropy(const Witness<kPuzzleWidth, kPuzzleHeight>& env) {
    kState = kIWS.ws;
    return kIWS.ws.path.empty() ? kEntropy.CalculateAdversarialEntropy(env, kState, 0)
                                : kEntropy.CalculatePartialAdversarialEntropy(env, kState, 0);
}

inline void UpdateEntropy(const Witness<kPuzzleWidth, kPuzzleHeight>& env) {
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
