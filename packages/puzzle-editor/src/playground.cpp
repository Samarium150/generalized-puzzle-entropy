#include <thread>

#include "entropy_util.h"
#include "handler.h"
#include "solution_util.h"

auto kAllSolutions = std::vector<WitnessState<kPuzzleWidth, kPuzzleHeight>>{};
auto kCurrentSolutionIndices = std::vector<std::size_t>{};
auto kIWS = InteractiveWitnessState<kPuzzleWidth, kPuzzleHeight>{};
auto kPuzzle = Witness<kPuzzleWidth, kPuzzleHeight>{};
auto kSolutionIndex = 0u;
auto kSolved = false;
auto kState = WitnessState<kPuzzleWidth, kPuzzleHeight>{};

bool ClickHandler(std::size_t /*id*/, int /*viewport*/, int /*x*/, int /*y*/, point3d p,  // NOLINT
                  tButtonType /*type*/, const tMouseEventType event) {
    if (event == kMouseUp && kPuzzle.Click(p, kIWS)) {
        if (kPuzzle.GoalTest(kIWS.ws)) {
            std::cout << "Solved!" << std::endl;
            kSolved = true;
        } else {
            std::cout << "Invalid solution" << std::endl;
            kIWS.Reset();
        }
    }
    if (event == kMouseMove) kPuzzle.Move(p, kIWS);
    return event != kMouseDrag;
}

void FrameHandler(const std::size_t id, unsigned /*viewport*/, void * /*data*/) {
    auto &display = GetContext(id)->display;
    kPuzzle.Draw(display);
    kIWS.IncrementTime();
    kPuzzle.Draw(display, kIWS);
    if (kSolved)
        display.DrawText("Solved!", point3d{1, 1}, Colors::black, 0.075, Graphics::textAlignRight,
                         Graphics::textBaselineBottom);
}

void KeyboardHandler(std::size_t /*id*/, tKeyboardModifier /*mod*/, const char key) {
    switch (key) {
        case 'l': {
            std::stringstream ss(getTextBuffer());
            auto witness = Witness<kPuzzleWidth, kPuzzleHeight>();
            ss >> witness;
            kPuzzle = witness;
#ifdef __EMSCRIPTEN_PTHREADS__
            std::thread(UpdateSolutionIndices).detach();
#else
            UpdateSolutionIndices();
#endif
            break;
        }
        case 'r':
            kIWS.Reset();
            kSolved = false;
            break;
        case 'v':
            if (!kCurrentSolutionIndices.empty()) {
                kSolutionIndex = (kSolutionIndex++) % kCurrentSolutionIndices.size();
                kIWS.ws = kAllSolutions[kCurrentSolutionIndices[kSolutionIndex]];
                kIWS.currState =
                    InteractiveWitnessState<kPuzzleWidth, kPuzzleHeight>::kWaitingRestart;
                kSolved = true;
            }
            break;
        default:
            break;
    }
}

void WindowHandler(const std::size_t id, const tWindowEventType type) {
    switch (type) {
        case kWindowCreated:
            InstallFrameHandler(FrameHandler, id, nullptr);
            SetNumPorts(id, 1);
            break;
        case kWindowDestroyed:
            RemoveFrameHandler(FrameHandler, id, nullptr);
            break;
    }
}

static void InitPuzzle() {
    GetAllSolutions(kPuzzle, kState, kAllSolutions);
    UpdateSolutionIndices();
}

static void InstallHandlers() {
    InstallKeyboardHandler(KeyboardHandler, "Load", "load a puzzle", kAnyModifier, 'l');
    InstallKeyboardHandler(KeyboardHandler, "Reset", "reset the path", kAnyModifier, 'r');
    InstallKeyboardHandler(KeyboardHandler, "Show Solution", "show a solution", kAnyModifier, 'v');
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
    RunHOGGUI(argc, argv, 640, 640);
    return 0;
}
