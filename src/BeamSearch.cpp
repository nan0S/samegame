#include "BeamSearch.hpp"

#include <cassert>
#include <algorithm>

Action BeamSearch::chooseAction(const State& game) {
    assert(!game.terminal());

    static constexpr int STATES = State::MAX_NEIGHBORS * BEAM_WIDTH;
    static std::array<State, STATES> beam;
    static std::unordered_set<State> next;
    next.clear();

    int beamSize = 1;
    beam[0] = game;
    ++totalNextCount;

    int depth = 0;
    float timeLimit = 49;

    for (Timer timer(timeLimit); timer.isTimeLeft(); ++depth) {
        assert(beamSize > 0);
        for (int i = 0; i < beamSize && timer.isTimeLeft(); ++i) {
            const auto& state = beam[i];
            state.getNeighbors(next, depth == 0);
        }

        if (!timer.isTimeLeft() && depth > 0)
            break;

        assert(!next.empty());
        totalNextCount += next.size();
        beamSize = 0;
        for (const auto& state : next)
            beam[beamSize++] = state;

        int considerCount = std::min(beamSize, BEAM_WIDTH);
        std::partial_sort(
            beam.data(),
            beam.data() + considerCount,
            beam.data() + beamSize,
            std::greater<State>());
        for (int i = 0; i < considerCount - 1; ++i)
            assert(beam[i].score >= beam[i + 1].score);

        next.clear();
        beamSize = considerCount;
    }

    assert(beamSize > 0);
    const auto& action = beam[0].firstAction;
    debug(action);
    assert(game.canPress(action.i, action.j));
    return action;
}
