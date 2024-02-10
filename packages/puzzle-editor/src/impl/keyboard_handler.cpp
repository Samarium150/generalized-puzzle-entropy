#include "entropy_util.h"
#include "handler.h"
#include "infer_util.h"
#include "solution_util.h"

auto kBest = std::vector<Witness<kPuzzleWidth, kPuzzleHeight>>{};
auto kCurrentBest = 0u;
auto kInferMutex = std::mutex();

static void ProvideSuggestion() {
    auto ss = std::stringstream();
    ss << R"({ "numBest": )" << kBest.size() << R"(, "numConstraints": )"
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

static void Calculate() {
    kBest.clear();
    if (kPuzzle.GetNumUnknownPathConstraints() != 0) {
        Infer(kPuzzle);
    }
    UpdateEntropy(kPuzzle);
    ProvideSuggestion();
}

static void Load(const std::size_t index) {
    kPuzzle = kBest[index];
    kIWS.Reset();
    kSolved = false;
    UpdateSolutionIndices();
    UpdateEntropy(kPuzzle);
    ProvideSuggestion();
}

void KeyboardHandler(const std::size_t id, tKeyboardModifier /*mod*/, const char key) {
    switch (key) {
        case '[':
            if (!kBest.empty()) {
                kCurrentBest = (kCurrentBest + kBest.size() - 1) % kBest.size();
                Load(kCurrentBest);
            }
            break;
        case ']':
            if (!kBest.empty()) {
                kCurrentBest = (++kCurrentBest) % kBest.size();
                Load(kCurrentBest);
            }
            break;
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
        case 'k': {
            std::stringstream ss(getTextBuffer());
            std::size_t index;
            ss >> index;
            if (index < kBest.size()) {
                kCurrentBest = index;
                Load(kCurrentBest);
            }
            break;
        }
        case 'l': {
            std::stringstream ss(getTextBuffer());
            auto witness = Witness<kPuzzleWidth, kPuzzleHeight>();
            ss >> witness;
            kPuzzle = witness;
            UpdateSolutionIndices();
            std::thread(UpdateEntropy, std::ref(kPuzzle)).detach();
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
