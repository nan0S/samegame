#include "NMCS.hpp"

#include <cassert>
#include <unistd.h>

std::ostream& operator<<(std::ostream& out, const NMCS::Sequence& sequence) {
    for (const auto& x : sequence)
        out << "(" << x << ") ";
    return out;
}

std::ostream& operator<<(std::ostream& out, const NMCS::Result& result) {
    out << "score: " << result.first << "\n";
    return out << "sequence: " << result.second;
}

NMCS::NMCS() : stoper(19900) {

}

void NMCS::init(const State& game) {

    stoper.start();
    Result bestResult = {-1, {}};

    while (stoper.isTimeLeft()) {
        auto result = nested(game, LEVEL);
        if (result.first > bestResult.first)
            bestResult = result;
        break;
    }

    bestSequence = bestResult.second;
    debug(bestResult.first);
}

NMCS::Result NMCS::nested(State state, int level) {
    assert(level >= 0);
    if (level == 0)
        return sample(state);

    Result bestResult = {-1, {}};
    Sequence visitedNodes;

    while (!state.terminal() && stoper.isTimeLeft()) {
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
        }

        if (currentResult.first >= bestResult.first) {
            visitedNodes.push_back(currentBestAction);
            bestResult = currentResult;
            bestResult.second.insert(bestResult.second.begin(),
                visitedNodes.begin(), visitedNodes.end());
        }
        else {
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
        const auto& action = actions[rand() % actions.size()];
        state.apply(action);
        sequence.push_back(action);
    }

    return {state.realscore, sequence};
}

Action NMCS::chooseAction(const State&) {
    static int seqIdx = 0;
    assert(seqIdx < int(bestSequence.size()));
    return bestSequence[seqIdx++];
}

void NMCS::end() const {

}
