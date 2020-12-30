#include "NMCS.hpp"

#include <cassert>
#include <unistd.h>
#include <cassert>

NMCS::Result NMCS::nested(State state, int level) {
    assert(level >= 0);
    if (level == 0)
        return sample(state);

    Result bestResult = sample(state);
    Sequence visitedNodes;

    while (!state.terminal() && timer.isTimeLeft()) {
        Result currentResult = {-1, {}};
        Action currentBestAction = {-1, -1};

        for (const auto& action : state.getActions()) {
            auto next = state;
            next.apply(action);

            auto result = nested(next, level - 1);
            if (result.first > currentResult.first) {
                currentBestAction = action;
                currentResult = result;
            }

            if (!timer.isTimeLeft())
                break;
        }

        if (currentResult.first > bestResult.first) {
            visitedNodes.push_back(currentBestAction);
            bestResult = currentResult;
            bestResult.second.insert(bestResult.second.begin(),
                visitedNodes.begin(), visitedNodes.end());
        }
        else {
            assert(bestResult.second.size() > visitedNodes.size());
            currentBestAction = bestResult.second[visitedNodes.size()];
            visitedNodes.push_back(currentBestAction);
        }

        state.apply(currentBestAction);
    }

    return bestResult;
}

NMCS::Result NMCS::sample(State state) {
    Sequence sequence;

    while (!state.terminal()) {
        auto actions = state.getActions();
        assert(!actions.empty());
        const auto& action = actions[rand() % actions.size()];
        state.apply(action);
        sequence.push_back(action);
    }

    return {state.realscore, sequence};
}

Action NMCS::chooseAction(const State& state, float timeLimit) {
    timer.set(timeLimit);
    timer.reset();

    while (timer.isTimeLeft()) {
        auto result = nested(state, LEVEL);
        if (result.first > bestResult.first)
            bestResult = result;
    }

    debug(bestResult.first);
    assert(!bestResult.second.empty());
    const auto bestAction = bestResult.second.front();
    bestResult.second.erase(bestResult.second.begin());

    return bestAction;
}
