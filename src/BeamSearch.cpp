                // if (board[i][j] == getTaboo())
#include "BeamSearch.hpp"

#include <cassert>
#include <algorithm>

Action BeamSearch::chooseAction(const State& game) {
    assert(!game.terminal());

    static constexpr int INFOS = State::MAX_NEIGHBORS * BEAM_WIDTH;
    static State beam[BEAM_WIDTH];
    static State next[BEAM_WIDTH];
    static Info infos[INFOS];
    static std::unordered_set<State> visited;

    int beamSize = 1;
    int infoSize = 0;
    beam[0] = game;
    ++totalNextCount;

    int depth = 0;
    float timeLimit = 48;

    for (Timer timer(timeLimit); timer.isTimeLeft(); ++depth) {
        assert(beamSize > 0);
        for (int i = 0; i < beamSize && timer.isTimeLeft(); ++i) {
            const auto& state = beam[i];
            infoSize += state.getNeighbors(infos + infoSize);
        }

        if (!timer.isTimeLeft() && depth > 0)
            break;

        assert(infoSize > 0);
        std::sort(infos, infos + infoSize, std::greater<Info>());
        beamSize = 0;

        for (int i = 0; i < infoSize && beamSize < BEAM_WIDTH; ++i) {
            const auto& info = infos[i];
            next[beamSize] = *info.state;
            auto& state = next[beamSize];

            if (info.action.i != -1)
                state.apply(info.action);
            if (depth == 0)
                state.firstAction = info.action;
            if (visited.insert(state).second)
                beamSize++;
        }

        visited.clear();
        totalNextCount += infoSize;
        infoSize = 0;
        std::swap(next, beam);
    }

    assert(beamSize > 0 && depth > 0);
    const auto& bestState = *std::max_element(beam, beam + beamSize, [](const State& s1, const State& s2){
        return s1.realscore < s2.realscore;
    });
    const auto& bestAction = bestState.firstAction;
    debug(bestState.realscore);
    debug(bestAction);
    assert(game.canPress(bestAction.i, bestAction.j));
    return bestAction;
}

void BeamSearch::end() const {
    debug(totalNextCount);
}
