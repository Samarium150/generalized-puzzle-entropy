#include <thread>

#include "entropy_util.h"
#include "handler.h"
#include "solution_util.h"
#include "the_witness_inference_rule.h"

auto kPuzzle = Witness<kPuzzleWidth, kPuzzleHeight>{};
auto kState = WitnessState<kPuzzleWidth, kPuzzleHeight>{};
auto kEntropy = WitnessPuzzleEntropy<kPuzzleWidth, kPuzzleHeight>{};
auto kEntropyInfo = EntropyInfo{};
auto kAdvEntropyInfo = AdversaryEntropyInfo{};
auto kIWS = InteractiveWitnessState<kPuzzleWidth, kPuzzleHeight>{};
auto kAllSolutions = std::vector<WitnessState<kPuzzleWidth, kPuzzleHeight>>{};
auto kCurrentSolutionIndices = std::vector<std::size_t>{};
auto kSolutionTree = std::vector<SolutionTreeNode>{};
auto kSolutionIndex = 0u;

static void InitPuzzle() {
    kEntropy.SetRelative(true).ruleSet.SetRules(
        kWitnessInferenceRules<kPuzzleWidth, kPuzzleHeight>);
    GetAllSolutions(kPuzzle, kState, kAllSolutions);
    BuildTree(kPuzzle, kAllSolutions, kSolutionTree);
    UpdateSolutionIndices();
    UpdateEntropy(kPuzzle);
}

static void InstallHandlers() {
    InstallKeyboardHandler(KeyboardHandler, "Prev Board", "Show next board.", kAnyModifier, '[');
    InstallKeyboardHandler(KeyboardHandler, "Next Board", "Show prev board", kAnyModifier, ']');
    InstallKeyboardHandler(KeyboardHandler, "Open Editor", "open editor panel", kAnyModifier, 'e');
    InstallKeyboardHandler(KeyboardHandler, "Load", "load a puzzle", kAnyModifier, 'l');
    InstallKeyboardHandler(KeyboardHandler, "Reset", "reset the path", kAnyModifier, 'r');
    InstallKeyboardHandler(KeyboardHandler, "Show Solution", "show a solution", kAnyModifier, 'v');
    InstallKeyboardHandler(KeyboardHandler, "Export", "export the puzzle", kAnyModifier, 'w');
    InstallKeyboardHandler(KeyboardHandler, "Suggest", "make a suggestion", kAnyModifier, 'z');
    InstallKeyboardHandler(KeyboardHandler, "Save", "Save SVG", kAnyModifier, 's');
    InstallMouseClickHandler(ClickHandler, static_cast<tMouseEventType>(kMouseMove | kMouseUp |
                                                                        kMouseDown | kMouseDrag));
    InstallWindowHandler(WindowHandler);
}

int main(const int argc, char **argv) {
#ifdef __EMSCRIPTEN_PTHREADS__
    std::thread(InitPuzzle).detach();
#else
    InitPuzzle();
#endif
    InstallHandlers();
    RunHOGGUI(argc, argv, 1280, 640);
    return 0;
}
