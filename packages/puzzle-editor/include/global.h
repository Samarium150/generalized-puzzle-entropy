#pragma once

#include "the_witness_puzzle_entropy.h"

constexpr double kInf = std::numeric_limits<double>::max();

struct RegionConstraintItem {
    WitnessRegionConstraint constraint{};
    point3d center{};
    float radius{};
};

extern std::vector<RegionConstraintItem> kRegionConstraintItems;

struct PathConstraintItem {
    WitnessPathConstraintType constraint{};
    point3d center{};
    float radius{};
};

extern std::vector<PathConstraintItem> kPathConstraintItems;

struct ColorItem {
    rgbColor color{};
    point3d center{};
    float radius{};
};

extern std::vector<ColorItem> kColorItems;

extern point3d kCursor;
extern int kCursorViewport;
extern bool kDrawEditor;
extern int kSelectedEditorItem;

constexpr int kPuzzleWidth = 4;
constexpr int kPuzzleHeight = 4;
constexpr unsigned kMaxNumUnknowns = 4;
extern Witness<kPuzzleWidth, kPuzzleHeight> kPuzzle;
extern Witness<kPuzzleWidth, kPuzzleHeight> kEditor;
extern WitnessState<kPuzzleWidth, kPuzzleHeight> kState;
extern InteractiveWitnessState<kPuzzleWidth, kPuzzleHeight> kIWS;
extern std::vector<WitnessState<kPuzzleWidth, kPuzzleHeight>> kAllSolutions;
extern std::vector<std::size_t> kCurrentSolutionIndices;
extern WitnessPuzzleEntropy<kPuzzleWidth, kPuzzleHeight> kEntropy;
extern EntropyInfo kEntropyInfo;
extern AdversaryEntropyInfo kAdvEntropyInfo;
extern bool kSolved;
extern unsigned kNumSolutions;
extern bool kShowNumSolutions;
extern unsigned kSolutionIndex;
extern unsigned kCurrentBest;
[[maybe_unused]] extern std::size_t kTotalWorkload;
extern std::size_t kCounter;
