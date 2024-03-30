#include <CLI/CLI.hpp>
#include <fstream>

#include "solution_util.h"
#include "the_witness_inference_rule.h"
#include "the_witness_puzzle_entropy.h"
#include "util.h"

static constexpr double kInf = std::numeric_limits<double>::max();
static constexpr int kPuzzleWidth = 4;
static constexpr int kPuzzleHeight = 4;
static auto kState = WitnessState<kPuzzleWidth, kPuzzleHeight>{};
static auto kEntropy = WitnessPuzzleEntropy<kPuzzleWidth, kPuzzleHeight>{};
static auto kAllSolutions = std::vector<WitnessState<kPuzzleWidth, kPuzzleHeight>>{};
auto kSolutionTree = std::vector<SolutionTreeNode>();

static void Init(Witness<kPuzzleWidth, kPuzzleHeight> &puzzle) {
    if (const auto [x, y] = puzzle.goal[0]; x < 0) {
        if (y == 0)
            puzzle.SetGoal(0, -1);
        else if (y == kPuzzleHeight)
            puzzle.SetGoal(0, kPuzzleHeight + 1);
    }
    auto empty = puzzle;
    empty.Clear();
    kState.Reset();
    kAllSolutions.clear();
    kSolutionTree.clear();
    GetAllSolutions(empty, kState, kAllSolutions);
    BuildTree(empty, kAllSolutions, kSolutionTree);
}

static auto Calculate(const Witness<kPuzzleWidth, kPuzzleHeight> &puzzle) {
    kState.Reset();
    const auto [entropy, i] = kEntropy.Calculate(puzzle, kState, 0, std::nullopt);
    kState.Reset();
    const auto [advEntropy, j, k] = kEntropy.CalculateAdversarialEntropy(puzzle, kState, 0);
    return std::make_pair(entropy, advEntropy);
}

int main(const int argc, char **argv) {
    CLI::App app;
    app.allow_windows_style_options();
    std::string filename;
    app.add_option("filename", filename, "input data file")->required();
    std::vector<int> inferenceFilter{};
    app.add_option("--inference_filter", inferenceFilter, "filter out specified inference rules")
        ->expected(0, kInferenceRuleCount - 1)
        ->required(false);
    CLI11_PARSE(app, argc, argv)

    kEntropy.SetRelative(true);
    kEntropy.ruleSet.SetRules(kWitnessInferenceRules<kPuzzleWidth, kPuzzleHeight>);
    std::for_each(inferenceFilter.begin(), inferenceFilter.end(),
                  [](const auto f) { kEntropy.ruleSet.DisableRule(f); });
    std::ifstream input(filename, std::ios::in);
    std::ofstream output("results.csv", std::ios::out);
    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }
    output << "id,timestamp,upvote,entropy,adv_entropy,solutions" << std::endl;
    unsigned i = 0;
    const auto total = count(input) - static_cast<std::size_t>(GetLastLine(input).empty());
    for (std::string line; std::getline(input, line);) {
        auto parts = std::vector<std::string>();
        split(line, '/', parts);
        if (parts.size() != 4) {
            std::cerr << "Invalid data" << std::endl;
            return 1;
        }
        std::cout << "processing: " << ++i << "/" << total << std::endl;
        std::istringstream iss(parts[3]);
        auto puzzle = Witness<4, 4>();
        iss >> puzzle;
        Init(puzzle);
        const auto [entropy, advEntropy] = Calculate(puzzle);
        const auto solutions = GetNumSolutions(puzzle, kAllSolutions);
        std::ostringstream oss;
        oss << parts[0] << "," << parts[1] << "," << parts[2] << ","
            << ((entropy == kInf) ? "inf" : std::to_string(entropy)) << "," << advEntropy << ","
            << solutions << std::endl;
        output << oss.str();
    }
    input.close();
    output.close();
    return 0;
}
