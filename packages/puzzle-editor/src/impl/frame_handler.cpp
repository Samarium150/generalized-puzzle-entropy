#include "handler.h"
#include "solver_util.h"

auto kRegionConstraintItems = std::vector<RegionConstraintItem>{
    {{kSeparation, 0, Colors::cyan}, point3d{-0.7, -0.52}, 0.1},
    {{kStar, 0, Colors::cyan}, point3d{-0.45, -0.52}, 0.1},
    {{kTriangle, 1, Colors::orange}, point3d{-0.2, -0.52}, 0.05},
    {{kTriangle, 2, Colors::orange}, point3d{0, -0.52}, 0.05},
    {{kTriangle, 3, Colors::orange}, point3d{0.27, -0.52}, 0.07},
    {{kUnknownRegionConstraint, 0, Colors::black}, point3d{0.5, -0.52}, 0.05}};
auto kPathConstraintItems =
    std::vector<PathConstraintItem>{{kNoPathConstraint, point3d{-0.7, -0.13}, 0.075},
                                    {kMustCross, point3d{-0.5, -0.13}, 0.075},
                                    {kCannotCross, point3d{-0.3, -0.13}, 0.075},
                                    {kUnknownPathConstraint, point3d{-0.1, -0.13}, 0.075}};
auto kColorItems = std::vector<ColorItem>{
    {Colors::black, point3d{-0.7, 0.3}, 0.1},   {Colors::cyan, point3d{-0.5, 0.3}, 0.1},
    {Colors::magenta, point3d{-0.3, 0.3}, 0.1}, {Colors::yellow, point3d{-0.1, 0.3}, 0.1},
    {Colors::red, point3d{0.1, 0.3}, 0.1},      {Colors::green, point3d{0.3, 0.3}, 0.1},
    {Colors::blue, point3d{0.5, 0.3}, 0.1},     {Colors::orange, point3d{0.7, 0.3}, 0.1},
};
auto kDrawEditor = false;
auto kNumSolutions = 0u;
auto kShowNumSolutions = true;
auto kEditor = Witness<kPuzzleWidth, kPuzzleHeight>{};
[[maybe_unused]] auto kTotalWorkload = 0ul;
auto kCounter = 0ul;
static auto kLastPosition = point3d{-1, -1};

static void DrawGameViewport(const std::size_t id) {
    auto &display = GetContext(id)->display;
    if (!kDrawEditor) {
        kPuzzle.Draw(display);
        kIWS.IncrementTime();
        kPuzzle.Draw(display, kIWS);
        if (kSolved)
            display.DrawText("Solved!", point3d{1, 1}, Colors::black, 0.075,
                             Graphics::textAlignRight, Graphics::textBaselineBottom);
        return;
    }
    kEditor.Draw(display);
    kEditor.Draw(display, kIWS.ws);
    if (kShowNumSolutions)
        display.DrawText("# of solutions: ", point3d{0.75, -0.95}, Colors::black, 0.075,
                         Graphics::textAlignRight, Graphics::textBaselineTop);
    if (!kBest.empty()) {
        display.DrawText(
            (std::to_string(kCurrentBest + 1) + std::string(" of ") + std::to_string(kBest.size()))
                .c_str(),
            point3d{1, 1}, Colors::black, 0.075, Graphics::textAlignRight,
            Graphics::textBaselineBottom);
    }
    if (kSelectedEditorItem != -1 && kCursorViewport == 0) {
        if (kSelectedEditorItem < kRegionConstraintItems.size()) {
            bool cursorInPuzzle = false;
            const auto &constraint = kRegionConstraintItems[kSelectedEditorItem].constraint;
            for (auto i = 0; i < kEditor.regionConstraintLocations.size(); ++i) {
                if (const auto &[position, rect] = kEditor.regionConstraintLocations[i];
                    PointInRect(kCursor, rect)) {
                    auto [x, y] = kEditor.GetRegionXYFromIndex(i);
                    if (position != kLastPosition) {
                        (constraint == kEditor.GetRegionConstraint(x, y))
                            ? kEditor.RemoveRegionConstraint(x, y)
                            : kEditor.AddRegionConstraint(x, y, constraint);
                        if (kShowNumSolutions)
                            kNumSolutions = GetNumSolutions(kEditor, kAllSolutions);
                    }
                    display.FrameRect(
                        rect,
                        (kShowNumSolutions && kNumSolutions == 0) ? Colors::red : Colors::gray,
                        0.01);
                    kEditor.DrawRegionConstraint(display, constraint, position);
                    kLastPosition = position;
                    cursorInPuzzle = true;
                    break;
                }
            }
            if (!cursorInPuzzle) kEditor.DrawRegionConstraint(display, constraint, kCursor);
        } else {
            for (auto i = 0; i < kEditor.pathConstraintLocations.size() - 1; ++i) {
                if (const auto &[position, rect] = kEditor.pathConstraintLocations[i];
                    PointInRect(kCursor, rect) &&
                    i != kPuzzleWidth * (kPuzzleHeight + 1) + (kPuzzleWidth + 1) * kPuzzleHeight) {
                    const auto &constraint =
                        kPathConstraintItems[kSelectedEditorItem - kRegionConstraintItems.size()]
                            .constraint;
                    kEditor.pathConstraints[i] =
                        (constraint == kEditor.pathConstraints[i]) ? kNoPathConstraint : constraint;
                    if (kShowNumSolutions) kNumSolutions = GetNumSolutions(kEditor, kAllSolutions);
                    display.FrameRect(
                        rect,
                        (kShowNumSolutions && kNumSolutions == 0) ? Colors::red : Colors::gray,
                        0.01);
                    kLastPosition = position;
                    break;
                }
            }
        }
        if (kShowNumSolutions)
            display.DrawText(std::to_string(kNumSolutions).c_str(), point3d{0.915, -0.95},
                             Colors::black, 0.075, Graphics::textAlignRight,
                             Graphics::textBaselineTop);
    } else {
        kLastPosition = point3d{-1, -1};
        if (kShowNumSolutions)
            display.DrawText(std::to_string(kCurrentSolutionIndices.size()).c_str(),
                             point3d{0.915, -0.95}, Colors::black, 0.075, Graphics::textAlignRight,
                             Graphics::textBaselineTop);
    }
}

static void DrawEditorViewport(const std::size_t id) {
    if (!kDrawEditor) return;

    auto &display = GetContext(id)->display;
    auto rect = Graphics::rect{-1.0, -1.0, 1.0, 1.0};

    display.FillRect(rect, Colors::gray);
    display.DrawText("Select a constraint", {-0.8, -0.8}, Colors::black, 0.1);
    display.DrawText("Region Constraint", {-0.8, -0.65}, Colors::black, 0.065);
    for (auto i = 0; i < kRegionConstraintItems.size(); ++i) {
        if (const auto &[constraint, c, radius] = kRegionConstraintItems[i];
            constraint.type != kUnknownRegionConstraint ||
            kEditor.GetNumUnknownConstraints() < kMaxNumUnknowns) {
            kEditor.DrawRegionConstraint(display, constraint, c);
            rect = {c, radius + 0.01f};
            if (i == kSelectedEditorItem)
                display.FrameRect(rect, Colors::white, 0.01);
            else if (kCursorViewport == 1 && PointInRect(kCursor, rect))
                display.FrameRect(rect, Colors::lightgray, 0.01);
        }
    }

    display.DrawText("Path Constraint", {-0.8, -0.27}, Colors::black, 0.065);
    for (auto i = 0; i < kPathConstraintItems.size(); ++i) {
        const auto &[constraint, c, radius] = kPathConstraintItems[i];
        rect = {c, radius + 0.01f};
        if (constraint != kUnknownPathConstraint ||
            kEditor.GetNumUnknownConstraints() < kMaxNumUnknowns) {
            if (i == kSelectedEditorItem - kRegionConstraintItems.size())
                display.FillRect(rect, Colors::green);
            else if (kCursorViewport == 1 && PointInRect(kCursor, rect))
                display.FillRect(rect, Colors::lightgray);
            else
                display.FillRect(rect, Colors::white);
        }
        switch (constraint) {
            case kMustCross:
                display.FillRect({c.x - radius, c.y - 0.025f, c.x + radius, c.y + 0.025f},
                                 kEditor.drawColor);
                display.FillNGon(c, 0.025, 6, 30, kPuzzle.backColor);
                break;
            case kCannotCross:
                display.FillRect({c.x - radius, c.y - 0.025f, c.x + radius, c.y + 0.025f},
                                 kEditor.drawColor);
                display.FillRect({c, 0.025}, kPuzzle.backColor);
                break;
            case kUnknownPathConstraint:
                if (kEditor.GetNumUnknownConstraints() < kMaxNumUnknowns)
                    display.DrawText("?", c, Colors::black, kEditor.lineWidth * 2.25f,
                                     Graphics::textAlignCenter, Graphics::textBaselineMiddle);
                break;
            default:
                break;
        }
    }

    display.DrawText("Change Color", {-0.8, 0.17}, Colors::black, 0.1);
    for (const auto &[color, c, _] : kColorItems) {
        rect = {c, 0.07};
        display.FillRect(rect, color);
        if (kCursorViewport == 1 && PointInRect(kCursor, rect))
            display.FrameRect(rect, Colors::lightgray, 0.01);
    }

    display.DrawText((kShowNumSolutions) ? "Hide # of solutions" : "Show # of solutions",
                     {-0.8, 0.75}, Colors::black, 0.1);
    rect = {-0.81, 0.63, 0.07, 0.75};
    if (kCursorViewport == 1 && PointInRect(kCursor, rect))
        display.FrameRect(rect, Colors::lightgray, 0.01);

    display.DrawText("Clear All", {0.8, 0.75}, Colors::black, 0.1, Graphics::textAlignRight,
                     Graphics::textBaselineBottom);
    rect = {0.42, 0.62, 0.81, 0.75};
    if (kCursorViewport == 1 && PointInRect(kCursor, rect))
        display.FrameRect(rect, Colors::lightgray, 0.01);
}

void FrameHandler(const std::size_t id, const unsigned viewport, void * /*data*/) {
    switch (viewport) {
        case 0:
            DrawGameViewport(id);
            break;
        case 1:
            kEditor = kPuzzle;
            DrawEditorViewport(id);
            break;
        default:
            break;
    }
}
