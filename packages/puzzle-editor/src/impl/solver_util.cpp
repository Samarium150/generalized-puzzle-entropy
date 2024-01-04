#include "solver_util.h"

void UpdateSolutionIndices() {
    kCurrentSolutionIndices.clear();
    for (auto i = 0; i < kAllSolutions.size(); ++i) {
        if (kPuzzle.GoalTest(kAllSolutions[i]))
            kCurrentSolutionIndices.emplace_back(i);
    }
    if (kSolved) {
        if (kCurrentSolutionIndices.empty())
            kIWS.Reset();
        else {
            kSolutionIndex = 0;
            kIWS.ws = kAllSolutions[kCurrentSolutionIndices[0]];
        }
    }
    UpdateEntropy(kPuzzle);
}
