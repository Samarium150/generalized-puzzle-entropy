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
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    kEntropy.ruleSet.SetRules(kWitnessInferenceRules<kPuzzleWidth, kPuzzleHeight>);
    std::ifstream input(std::string(argv[1]), std::ios::in);
    std::ofstream output("result.csv", std::ios::out);
    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }
    output << "id,entropy,adv_entropy,up_votes,solves" << std::endl;
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
        std::istringstream iss(parts[1]);
        auto puzzle = Witness<4, 4>();
        iss >> puzzle;
        Init(puzzle);
        auto [entropy, advEntropy] = Calculate(puzzle);
        std::ostringstream oss;
        oss << parts[0] << "," << ((entropy == kInf) ? "inf" : std::to_string(entropy)) << ","
            << advEntropy << "," << parts[2] << "," << parts[3] << std::endl;
        output << oss.str();
    }
    input.close();
    output.close();
    return 0;
}
