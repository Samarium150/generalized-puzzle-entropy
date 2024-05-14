#include <thread>

#include "entropy_util.h"
#include "handler.h"

auto kCursor = point3d{0, 0, 0};
auto kCursorViewport = 0;
auto kSelectedEditorItem = -1;
auto kSolved = false;

bool ClickHandler(std::size_t /*id*/, const int viewport, int /*x*/, int /*y*/,
                  point3d p,  // NOLINT
                  tButtonType /*type*/, const tMouseEventType event) {
    switch (viewport) {
        case 0:
            if (!kDrawEditor) {
                if (event == kMouseUp && kPuzzle.Click(p, kIWS)) {
                    if (kPuzzle.GoalTest(kIWS.ws)) {
                        std::cout << "Solved!" << std::endl;
                        kSolved = true;
                    } else {
                        std::cout << "Invalid solution" << std::endl;
                        kIWS.Reset();
                    }
                }
                if (event == kMouseDrag || event == kMouseMove) kPuzzle.Move(p, kIWS);
            } else if (event == kMouseDown && kSelectedEditorItem != -1 &&
                       viewport == kCursorViewport) {
                if (kSelectedEditorItem < kRegionConstraintItems.size()) {
                    for (auto i = 0; i < kPuzzle.regionConstraintLocations.size(); ++i) {
                        if (PointInRect(p, kPuzzle.regionConstraintLocations[i].second)) {
                            auto [x, y] = kPuzzle.GetRegionXYFromIndex(i);

                            if (const auto &constraint =
                                    kRegionConstraintItems[kSelectedEditorItem].constraint;
                                constraint == kPuzzle.GetRegionConstraint(x, y))
                                kPuzzle.RemoveRegionConstraint(x, y);
                            else
                                kPuzzle.AddRegionConstraint(x, y, constraint);
                            break;
                        }
                    }
                } else {
                    for (auto i = 0; i < kPuzzle.pathConstraintLocations.size() - 1; ++i) {
                        if (PointInRect(p, kPuzzle.pathConstraintLocations[i].second) &&
                            i != kPuzzleWidth * (kPuzzleHeight + 1) +
                                     (kPuzzleWidth + 1) * kPuzzleHeight) {
                            if (const auto constraint =
                                    kPathConstraintItems[kSelectedEditorItem -
                                                         kRegionConstraintItems.size()]
                                        .constraint;
                                constraint == kEditor.pathConstraints[i])
                                kPuzzle.RemovePathConstraint(i);
                            else
                                kPuzzle.AddPathConstraint(i, constraint);
                            break;
                        }
                    }
                }
                if (kPuzzle.GetNumUnknownConstraints() >= kMaxNumUnknowns) kSelectedEditorItem = -1;
#ifdef __EMSCRIPTEN_PTHREADS__
                std::thread(UpdateSolutionIndices).detach();
#else
                UpdateSolutionIndices();
#endif
            }
            break;
        case 1:
            if (event == kMouseDown && viewport == kCursorViewport) {
                kEditor.UpdateUnknownConstraints();
                auto constraintSelected = false;
                for (auto i = 0; i < kRegionConstraintItems.size(); ++i) {
                    if (const auto &[constraint, c, radius] = kRegionConstraintItems[i];
                        PointInRect(p, {c, radius})) {
                        if (constraint.type != kUnknownRegionConstraint ||
                            kEditor.GetNumUnknownRegionConstraints() < kMaxNumUnknowns) {
                            kSelectedEditorItem = i;
                            constraintSelected = true;
                        }
                        break;
                    }
                }
                for (auto i = 0; i < kPathConstraintItems.size(); ++i) {
                    if (const auto &[constraint, c, radius] = kPathConstraintItems[i];
                        PointInRect(p, {c, radius})) {
                        if (constraint != kUnknownPathConstraint ||
                            kEditor.GetNumUnknownPathConstraints() < kMaxNumUnknowns) {
                            kSelectedEditorItem =
                                i + static_cast<int>(kRegionConstraintItems.size());
                            constraintSelected = true;
                        }
                        break;
                    }
                }
                auto colorSelected = false;
                for (const auto &[color, c, radius] : kColorItems) {
                    if (PointInRect(p, {c, radius})) {
                        colorSelected = true;
                        kRegionConstraintItems[0].constraint.color = color;
                        kRegionConstraintItems[1].constraint.color = color;
                        break;
                    }
                }
                if (!constraintSelected && !colorSelected) kSelectedEditorItem = -1;
                if (PointInRect(p, {-0.81, 0.63, 0.07, 0.75}))
                    kShowNumSolutions = !kShowNumSolutions;
                if (PointInRect(p, {0.42, 0.62, 0.81, 0.75})) {
                    kPuzzle.Clear();
                    kIWS.Reset();
                    kSolved = false;
                    UpdateSolutionIndices();
#ifdef __EMSCRIPTEN_PTHREADS__
                    std::thread(UpdateEntropy, std::ref(kPuzzle)).detach();
#else
                    UpdateEntropy(kPuzzle);
#endif
                }
            }
            break;
        default:
            break;
    }
    kCursor = p;
    kCursorViewport = viewport;
    return event != kMouseDrag;
}
