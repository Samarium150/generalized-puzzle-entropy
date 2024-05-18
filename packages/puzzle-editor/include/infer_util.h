#pragma once

#include <thread>

#include "entropy_util.h"
#include "solution_util.h"

extern std::mutex kInferMutex;
extern std::unordered_map<std::string, double> kInferRecord;
extern std::vector<Witness<kPuzzleWidth, kPuzzleHeight>> kBest;

template <int width, int height>
[[maybe_unused]] void InferHelper(const Witness<width, height>& puzzle, const unsigned start,
                                  const unsigned end, const std::size_t nrs, const std::size_t rcc,
                                  const std::size_t nps, const std::size_t pcc,
                                  const std::vector<rgbColor>& colors) {
    auto rc = std::vector<unsigned>(nrs);
    auto pc = std::vector<unsigned>(nps);

    auto carry = start;
    for (auto& value : rc) {
        value += carry % rcc;
        carry /= rcc;
    }
    for (auto i = start; i < end; ++i) {
        unsigned slot;
        do {
            auto newPuzzle = puzzle;
            newPuzzle.RemoveUnknownConstraints(rc, pc, colors);
            if (const auto value = GetNumSolutions(newPuzzle, kAllSolutions); value != 0) {
                kInferMutex.lock();
                auto state = WitnessState<width, height>{};
                kInferRecord[newPuzzle.SaveToHashString()] =
                    kEntropy.CalculateTotalSolutionInformation(newPuzzle, state);
                kBest.push_back(newPuzzle);
                kInferMutex.unlock();
            }
            kInferMutex.lock();
            kCounter++;
            kInferMutex.unlock();
            slot = 0;
            // Increment combination of the second set
            while (slot < nps) {
                if (++pc[slot] < pcc) break;
                pc[slot++] = 0;
            }
        } while (slot < nps);

        // Reset the second set
        std::fill(pc.begin(), pc.end(), 0);

        // Increment combination of the first set
        slot = 0;
        while (slot < rc.size()) {
            if (++rc[slot] < rcc) break;
            rc[slot++] = 0;
        }
    }
}

template <int width, int height>
void Infer(const Witness<width, height>& puzzle) {
    kBest.emplace_back(puzzle);
    kTotalWorkload = 0;
    const auto numRegionSlot = puzzle.GetNumUnknownRegionConstraints();
    puzzle.CountColors();
    auto colors = std::vector<rgbColor>();
    std::transform(puzzle.colorMap.cbegin(), puzzle.colorMap.cend(), std::back_inserter(colors),
                   [](const auto& p) { return p.first; });
    switch (numRegionSlot) {
        case 0:
            colors.clear();
            break;
        case 1:
            if (colors.size() == 1)
                colors.emplace_back(kColorItems[random() % kColorItems.size()].color);
            break;
        default:
            while (colors.size() < 2)
                colors.emplace_back(kColorItems[random() % kColorItems.size()].color);
    }
    const auto regionConstraintChoices = 3 + 2 * colors.size();
    const auto rca = static_cast<std::size_t>(std::pow(regionConstraintChoices, numRegionSlot));

    const auto numPathSlot = puzzle.GetNumUnknownPathConstraints();
    const auto pathConstraintChoices = kPathConstraintItems.size() - 1;
    const auto pca = static_cast<std::size_t>(std::pow(pathConstraintChoices, numPathSlot));
    kTotalWorkload = rca * pca;
#ifdef __EMSCRIPTEN_PTHREADS__
    const auto numThreads = std::thread::hardware_concurrency();
    const auto combPerT = rca / numThreads;
    std::vector<std::thread> threads;
    for (auto i = 0; i < numThreads; ++i) {
        unsigned start = i * combPerT;
        unsigned end = (i == numThreads - 1) ? rca : start + combPerT;
        threads.emplace_back(InferHelper<width, height>, std::ref(puzzle), start, end,
                             numRegionSlot, regionConstraintChoices, numPathSlot,
                             pathConstraintChoices, std::ref(colors));
    }
    std::for_each(threads.begin(), threads.end(), [](auto& t) { t.join(); });
#else
    auto rc = std::vector<unsigned>(numRegionSlot);
    auto pc = std::vector<unsigned>(numPathSlot);
    while (true) {
        unsigned slot;
        while (true) {
            auto newPuzzle = puzzle;
            newPuzzle.RemoveUnknownConstraints(rc, pc, colors);
            auto state = kState;
            if (const auto value = GetNumSolutions(newPuzzle, kAllSolutions); value != 0) {
                auto state = WitnessState<width, height>{};
                kInferRecord[newPuzzle.SaveToHashString()] =
                    kEntropy.CalculateTotalSolutionInformation(newPuzzle, state);
                kBest.push_back(newPuzzle);
            }
            kCounter++;

            slot = 0;
            while (slot < numRegionSlot) {
                if (++rc[slot] < regionConstraintChoices) break;
                rc[slot++] = 0;
            }
            if (slot == numRegionSlot) break;
        }
        std::fill(begin(rc), end(rc), 0);
        slot = 0;
        while (slot < numPathSlot) {
            if (++pc[slot] < pathConstraintChoices) break;
            pc[slot++] = 0;
        }
        if (slot == numPathSlot) break;
    }
#endif
    std::sort(kBest.begin(), kBest.end(), [](const auto& a, const auto& b) {
        return kInferRecord[a.SaveToHashString()] > kInferRecord[b.SaveToHashString()];
    });
    kInferRecord.clear();
    kTotalWorkload = 0;
}
