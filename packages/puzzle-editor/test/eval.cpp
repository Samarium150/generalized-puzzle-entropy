#include <CLI/CLI.hpp>
#include <filesystem>
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

static void Init(const Witness<kPuzzleWidth, kPuzzleHeight> &puzzle) {
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
    const auto [muse, i] = kEntropy.SetRelative(false).Calculate(puzzle, kState, 0, std::nullopt);
    kState.Reset();
    const auto [remuse, j] = kEntropy.SetRelative(true).Calculate(puzzle, kState, 0, std::nullopt);
    kState.Reset();
    const auto [advEntropy, h, k] = kEntropy.CalculateAdversarialEntropy(puzzle, kState, 0);
    kState.Reset();
    return std::make_tuple(muse, remuse, advEntropy);
}

[[maybe_unused]] static void RecordSolutions(const std::string &id,
                                             const Witness<kPuzzleWidth, kPuzzleHeight> &puzzle) {
    kState.Reset();
    std::vector<WitnessState<kPuzzleWidth, kPuzzleHeight>> solutions;
    std::vector<WitnessAction> actions;
    DFS(puzzle, kState, solutions);
    std::filesystem::create_directory(id);
    for (auto i = 0; i < solutions.size(); ++i) {
        std::ofstream output(id + "/" + std::to_string(i) + ".txt", std::ios::out);
        auto &solution = solutions[i];
        puzzle.GetActionSequence(solution, actions);
        for (const auto &action : actions) {
            switch (action) {
                case kUp:
                    output << 0 << std::endl;
                break;
                case kDown:
                    output << 1 << std::endl;
                break;
                case kRight:
                    output << 2 << std::endl;
                break;
                case kLeft:
                    output << 3 << std::endl;
                break;
                default:
                    break;
            }
        }
        output.close();
    }
}

int main(const int argc, char **argv) {
    CLI::App app;
    app.allow_windows_style_options();
    std::string filename;
    app.add_option("filename", filename, "input data file")->required();
    std::vector<int> inferenceFilter{};
    app.add_option("--inference_filter", inferenceFilter, "disbale specified inference rules")
        ->expected(0, kInferenceRuleCount)
        ->required(false);
    CLI11_PARSE(app, argc, argv)

    // kEntropy.SetBase2(true);
    kEntropy.ruleSet.SetRules(kWitnessInferenceRules<kPuzzleWidth, kPuzzleHeight>);
    std::for_each(inferenceFilter.begin(), inferenceFilter.end(), [](const auto f) {
        if (f >= 0 && f < kInferenceRuleCount) kEntropy.ruleSet.DisableRule(f);
    });
    std::ifstream input(filename, std::ios::in);
    std::ofstream output("results.csv", std::ios::out);
    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }
    output << "id,timestamp,upvote,muse,remuse,adv_entropy,tsi,solutions,solves" << std::endl;
    unsigned i = 0;
    const auto total = count(input) - static_cast<std::size_t>(GetLastLine(input).empty());
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
        const auto [muse, remuse, advEntropy] = Calculate(puzzle);
        const auto tsi = kEntropy.CalculateTotalSolutionInformation(puzzle, kState);
        const auto solutions = GetNumSolutions(puzzle, kAllSolutions);
        std::ostringstream oss;
        oss << parts[0] << "," << parts[1] << "," << parts[2] << ","
            << ((muse == kInf) ? "inf" : std::to_string(muse)) << ","
            << ((remuse == kInf) ? "inf" : std::to_string(remuse)) << "," << advEntropy << ","
            << ((tsi == kInf) ? "inf" : std::to_string(tsi)) << "," << solutions << "," << parts[3]
            << std::endl;
        output << oss.str();
    }
    input.close();
    output.close();
    return 0;
}
