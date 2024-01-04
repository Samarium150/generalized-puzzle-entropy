#pragma once

#include "PuzzleEntropy.h"
#include "Witness.h"
#include "the_witness_inference_rule.h"
#include "the_witness_puzzle_entropy.h"

constexpr double kInf = std::numeric_limits<double>::max();

struct RegionConstraintItem {
    WitnessRegionConstraint constraint{};
    Graphics::point center{};
    float radius{};
};

extern std::vector<RegionConstraintItem> kRegionConstraintItems;

struct PathConstraintItem {
    WitnessPathConstraintType constraint{};
    Graphics::point center{};
    float radius{};
};

extern std::vector<PathConstraintItem> kPathConstraintItems;

struct ColorItem {
    rgbColor color{};
    Graphics::point center{};
    float radius{};
};

extern std::vector<ColorItem> kColorItems;

extern Graphics::point kCursor;
extern int kCursorViewport;
extern bool kDrawEditor;
extern int kSelectedEditorItem;

constexpr int kPuzzleWidth = 4;
constexpr int kPuzzleHeight = 4;
extern Witness<kPuzzleWidth, kPuzzleHeight> kPuzzle;
extern Witness<kPuzzleWidth, kPuzzleHeight> kEditor;
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
