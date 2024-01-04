#include <thread>

#include "global.h"
#include "handler.h"
#include "solver_util.h"

static void Calculate() {
    UpdateEntropy(kPuzzle);
    auto ss = std::stringstream();
    ss << R"({ "numConstraints": )"
       << std::count_if(kPuzzle.pathConstraints.cbegin(), kPuzzle.pathConstraints.cend(),
                        [](const auto &constraint) { return constraint != kNoPathConstraint; }) +
              std::accumulate(
                  kPuzzle.regionConstraints.cbegin(), kPuzzle.regionConstraints.cend(), 0u,
                  [](const auto &sum, const auto &constraints) {
                      return sum + std::count_if(constraints.cbegin(), constraints.cend(),
                                                 [](const auto &constraint) {
                                                     return constraint.type != kNoRegionConstraint;
                                                 });
                  })
       << R"(, "numSolutions": )" << GetNumSolutions(kPuzzle, kAllSolutions) << R"(, "entropy": )"
       << ((kEntropyInfo.value == kInf) ? "inf" : std::to_string(kEntropyInfo.value))
       << R"(, "advEntropy": )" << kAdvEntropyInfo.value << " }";
    submitTextToBuffer(ss.str().c_str());
}

void KeyboardHandler(std::size_t id, tKeyboardModifier /*mod*/, char key) {
    switch (key) {
        case 'e':
            kIWS.Reset();
            kSolved = false;
            if (!kDrawEditor) {
                kDrawEditor = true;
                MoveViewport(id, 1, {0.0f, -1.0f, 1.0f, 1.0f});
            } else {
                MoveViewport(id, 1, {1.0f, -1.0f, 2.0f, 1.0f});
                MoveViewport(id, 2, {1.0f, 0.0f, 2.0f, 2.0f});
                kDrawEditor = false;
                kSelectedEditorItem = -1;
            }
            break;
        case 'l': {
            std::stringstream ss(getTextBuffer());
            auto witness = Witness<kPuzzleWidth, kPuzzleHeight>();
            ss >> witness;
            kPuzzle = witness;
            UpdateSolutionIndices();
            break;
        }
        case 'r':
            kIWS.Reset();
            kSolved = false;
            break;
        case 'v':
            if (!kCurrentSolutionIndices.empty()) {
                kIWS.ws = kAllSolutions[kCurrentSolutionIndices[(kSolutionIndex++) %
                                                                kCurrentSolutionIndices.size()]];
                kIWS.currState =
                    InteractiveWitnessState<kPuzzleWidth, kPuzzleHeight>::kWaitingRestart;
                kSolved = true;
            }
            break;
        case 'w':
            submitTextToBuffer(std::string(kPuzzle).c_str());
            break;
        case 'z': {
            submitTextToBuffer("");
            Calculate();
            break;
        }
        default:
            break;
    }
}
