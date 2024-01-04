#include <thread>

#include "handler.h"
#include "solver_util.h"
#include "the_witness_inference_rule.h"

auto kPuzzle = Witness<kPuzzleWidth, kPuzzleHeight>{};
auto kEntropy = WitnessPuzzleEntropy<kPuzzleWidth, kPuzzleHeight>{};
auto kEntropyInfo = EntropyInfo{};
auto kAdvEntropyInfo = AdversaryEntropyInfo{};
auto kIWS = InteractiveWitnessState<kPuzzleWidth, kPuzzleHeight>{};
auto kAllSolutions = std::vector<WitnessState<kPuzzleWidth, kPuzzleHeight>>{};
auto kCurrentSolutionIndices = std::vector<std::size_t>{};
auto kSolutionTree = std::vector<SolutionTreeNode>{};
auto kSolutionIndex = 0u;

static void InitPuzzle() {
    kEntropy.ruleSet.SetRules(
        kWitnessInferenceRules<kPuzzleWidth, kPuzzleHeight>);
    GetAllSolutions(kPuzzle, kAllSolutions);
    BuildTree(kPuzzle, kAllSolutions, kSolutionTree);
    UpdateSolutionIndices();
    UpdateEntropy(kPuzzle);
}

static void InstallHandlers() {
    InstallKeyboardHandler(KeyboardHandler, "Open Editor", "open editor panel",
                           kAnyModifier, 'e');
    InstallKeyboardHandler(KeyboardHandler, "Load", "load a puzzle",
                           kAnyModifier, 'l');
    InstallKeyboardHandler(KeyboardHandler, "Reset", "reset the path",
                           kAnyModifier, 'r');
    InstallKeyboardHandler(KeyboardHandler, "Show Solution", "show a solution",
                           kAnyModifier, 'v');
    InstallKeyboardHandler(KeyboardHandler, "Export", "export the puzzle",
                           kAnyModifier, 'w');
    InstallKeyboardHandler(KeyboardHandler, "Suggest", "make a suggestion",
                           kAnyModifier, 'z');
    InstallMouseClickHandler(
        ClickHandler, static_cast<tMouseEventType>(kMouseMove | kMouseUp |
                                                   kMouseDown | kMouseDrag));
    InstallWindowHandler(WindowHandler);
}

int main(int argc, char **argv) {
    std::thread(InitPuzzle).detach();
    InstallHandlers();
    RunHOGGUI(argc, argv, 1280, 640);
    return 0;
}
