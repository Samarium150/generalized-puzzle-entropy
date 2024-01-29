#pragma once

#include <thread>

#include "global.h"

template <int width, int height>
void DFS(const Witness<width, height>& env, WitnessState<width, height>& state,
         std::vector<WitnessState<width, height>>& solutions) {
    if (env.GoalTest(state)) {
        solutions.push_back(state);
        return;
    }
    auto& actions = *env.actionCache.getItem();
    env.GetActions(state, actions);
    for (const auto& action : actions) {
        env.ApplyAction(state, action);
        DFS(env, state, solutions);
        env.UndoAction(state, action);
    }
    env.actionCache.returnItem(&actions);
}

template <int width, int height>
void GetAllSolutions(const Witness<width, height>& env, WitnessState<width, height>& state,
                     std::vector<WitnessState<width, height>>& puzzles) {
    DFS(env, state, puzzles);
}

template <int width, int height>
auto GetNumSolutions(const Witness<width, height>& puzzle,
                     const std::vector<WitnessState<width, height>>& solutions) {
    return std::accumulate(solutions.begin(), solutions.end(), 0ul,
                           [&puzzle](const std::size_t& sum, const auto& solution) {
                               return sum + static_cast<std::size_t>(puzzle.GoalTest(solution));
                           });
}

inline auto GetCurrentEntropy(const Witness<kPuzzleWidth, kPuzzleHeight>& env) {
    kState = kIWS.ws;
    return kEntropy.SetRelative(true).Calculate(env, kState, 0, std::nullopt);
}

inline auto GetCurrentAdvEntropy(const Witness<kPuzzleWidth, kPuzzleHeight>& env) {
    kState = kIWS.ws;
    return kIWS.ws.path.empty() ? kEntropy.CalculateAdversarialEntropy(env, kState, 0)
                                : kEntropy.CalculatePartialAdversarialEntropy(env, kState, 0);
}

inline void UpdateEntropy(const Witness<kPuzzleWidth, kPuzzleHeight>& env) {
    kEntropyInfo = GetCurrentEntropy(env);
    kAdvEntropyInfo = GetCurrentAdvEntropy(env);
}

inline void UpdateSolutionIndices() {
    kCurrentSolutionIndices.clear();
    for (auto i = 0; i < kAllSolutions.size(); ++i) {
        if (kPuzzle.GoalTest(kAllSolutions[i])) kCurrentSolutionIndices.emplace_back(i);
    }
    if (kSolved) {
        if (kCurrentSolutionIndices.empty())
            kIWS.Reset();
        else {
            kSolutionIndex = 0;
            kIWS.ws = kAllSolutions[kCurrentSolutionIndices[0]];
        }
    }
}

extern std::mutex kInferMutex;

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
            if (GetNumSolutions(newPuzzle, kAllSolutions) != 0) {
                kInferMutex.lock();
                kBest.push_back(newPuzzle);
                kInferMutex.unlock();
            }
            kInferMutex.lock();
            kCounter++;
            kInferMutex.unlock();
            slot = 0;
            // Increment combination of the second set
            while (slot < nps) {
                if (++pc[slot] < pcc)
                    break;
                else
                    pc[slot++] = 0;
            }
        } while (slot < nps);

        // Reset the second set
        std::fill(pc.begin(), pc.end(), 0);

        // Increment combination of the first set
        slot = 0;
        while (slot < rc.size()) {
            if (++rc[slot] < rcc)
                break;
            else
                rc[slot++] = 0;
        }
    }
}

template <int width, int height>
void Infer(const Witness<width, height>& puzzle) {
    kBest.emplace_back(kPuzzle);
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
    std::thread([]() {
        std::sort(kBest.begin() + 1, kBest.end(), [](const auto& a, const auto& b) {
            return GetNumSolutions(a, kAllSolutions) < GetNumSolutions(b, kAllSolutions);
        });
    }).detach();
#else
    auto rc = std::vector<unsigned>(numRegionSlot);
    auto pc = std::vector<unsigned>(numPathSlot);
    while (true) {
        unsigned slot;
        while (true) {
            auto newPuzzle = env;
            newPuzzle.RemoveUnknownConstraints(rc, pc, colors);
            auto state = kState;
            if (GetNumSolutions(newPuzzle, kAllSolutions) != 0) {
                kBest.push_back(newPuzzle);
            }
            kCounter++;

            slot = 0;
            while (slot < numRegionSlot) {
                if (++rc[slot] < regionConstraintChoices)
                    break;
                else
                    rc[slot++] = 0;
            }
            if (slot == numRegionSlot) break;
        }
        std::fill(begin(rc), end(rc), 0);
        slot = 0;
        while (slot < numPathSlot) {
            if (++pc[slot] < pathConstraintChoices)
                break;
            else
                pc[slot++] = 0;
        }
        if (slot == numPathSlot) break;
    }
    std::sort(kBest.begin() + 1, kBest.end(), [](auto& a, auto& b) {
        return GetNumSolutions(a, kAllSolutions) < GetNumSolutions(b, kAllSolutions);
    });
#endif
    kTotalWorkload = 0;
}
