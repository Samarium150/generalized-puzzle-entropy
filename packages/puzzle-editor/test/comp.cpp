#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include "solution_util.h"
#include "the_witness_inference_rule.h"
#include "the_witness_puzzle_entropy.h"
#include "util.h"

using json = nlohmann::json;

static constexpr int kPuzzleWidth = 4;
static constexpr int kPuzzleHeight = 4;
static auto kState = WitnessState<kPuzzleWidth, kPuzzleHeight>{};
static auto kEntropy = WitnessPuzzleEntropy<kPuzzleWidth, kPuzzleHeight>{};
static auto kEntropy2 = WitnessPuzzleEntropy<kPuzzleWidth, kPuzzleHeight>{};
static auto kAllSolutions = std::vector<WitnessState<kPuzzleWidth, kPuzzleHeight>>{};
auto kSolutionTree = std::vector<SolutionTreeNode>();

static void Init(const Witness<kPuzzleWidth, kPuzzleHeight>& puzzle) {
    auto empty = puzzle;
    empty.Clear();
    kState.Reset();
    kAllSolutions.clear();
    kSolutionTree.clear();
    GetAllSolutions(empty, kState, kAllSolutions);
    BuildTree(empty, kAllSolutions, kSolutionTree);
}

static std::string encode(const std::vector<WitnessAction>& actions) {
    std::stringstream ss;
    for (const auto& action : actions) {
        switch (action) {
            case kUp:
                ss << 0;
                break;
            case kDown:
                ss << 1;
                break;
            case kRight:
                ss << 2;
                break;
            case kLeft:
                ss << 3;
                break;
            default:
                break;
        }
    }
    return ss.str();
}

int main() {
    kEntropy.ruleSet.SetRules(kWitnessInferenceRules<kPuzzleWidth, kPuzzleHeight>);
    kEntropy.ruleSet.DisableRule(4);
    kEntropy.ruleSet.DisableRule(5);

    kEntropy2.ruleSet.SetRules(kWitnessInferenceRules<kPuzzleWidth, kPuzzleHeight>);
    kEntropy2.ruleSet.DisableRule(2);
    kEntropy2.ruleSet.DisableRule(4);

    auto lmd = [](const WitnessAction& action) {
        switch (action) {
            case kUp:
                return 0;
            case kDown:
                return 1;
            case kRight:
                return 2;
            case kLeft:
                return 3;
            default:
                return -1;
        }
    };

    std::ifstream input("2022-11-29.txt", std::ios::in);
    std::filesystem::create_directory("puzzles_test_rcr");
    unsigned i = 0;
    const auto total = count(input) - static_cast<std::size_t>(GetLastLine(input).empty());
    auto actions = std::vector<WitnessAction>{};
    auto currActions = std::vector<WitnessAction>{};
    for (std::string line; std::getline(input, line);) {
        auto parts = std::vector<std::string>();
        split(line, '/', parts);
        if (parts.size() != 5) {
            std::cerr << "Invalid data" << std::endl;
            return 1;
        }
        std::cout << "processing: " << parts[0] << " (" << ++i << "/" << total << ")" << std::endl;
        std::istringstream iss(parts[4]);
        auto puzzle = Witness<kPuzzleWidth, kPuzzleHeight>();
        iss >> puzzle;
        Init(puzzle);
        kState.Reset();
        auto solutions = std::vector<WitnessState<kPuzzleWidth, kPuzzleHeight>>{};
        DFS(puzzle, kState, solutions);
        auto record =
            std::unordered_map<std::string, std::pair<std::vector<int>, std::vector<int>>>{};
        for (const auto& solution : solutions) {
            kState.Reset();
            puzzle.GetActionSequence(solution, actions);
            for (auto idx = 0; idx < actions.size() - 1; ++idx) {
                puzzle.ApplyAction(kState, actions[idx]);
                if (const auto id = encode(
                        std::vector<WitnessAction>{actions.begin(), actions.begin() + idx + 1});
                    record.find(id) == record.end()) {
                    puzzle.GetActions(kState, currActions);
                    auto originalActions = currActions;
                    auto originalState = kState;
                    kEntropy.ruleSet.FilterActions(puzzle, kState, currActions);
                    auto targetActions = originalActions;
                    auto targetState = kState;
                    kEntropy2.ruleSet.FilterActions(puzzle, targetState, targetActions);
                    if (currActions[0] != kEnd && originalActions.size() == currActions.size() &&
                        originalActions.size() != targetActions.size()) {
                        auto v1 = std::vector<int>{};
                        std::transform(targetActions.begin(), targetActions.end(),
                                       std::back_inserter(v1), lmd);
                        auto v2 = std::vector<int>{};
                        std::transform(originalActions.begin(), originalActions.end(),
                                       std::back_inserter(v2), lmd);
                        record[id] = std::make_pair(v1, v2);
                    }
                }
            }
        }
        if (record.empty()) std::cout << "No pruned actions found for " << parts[0] << std::endl;
        json j = record;
        std::ofstream ofs("puzzles_test/" + parts[0] + ".json", std::ios::out);
        ofs << j.dump(2);
        ofs.close();
    }
    input.close();
    return 0;
}
