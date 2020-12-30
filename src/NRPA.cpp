#include "NRPA.hpp"

#include <cassert>

bool operator<(const NRPA::Result& r1, const NRPA::Result& r2) {
    return r1.first < r2.first;
}

Action NRPA::chooseAction(const State& state, float timeLimit) {
    timer.set(timeLimit);
    timer.reset();
    initialState = &state;

    while (timer.isTimeLeft()) {
        auto result = nested(LEVEL, initialPolicy());
        if (result > bestResult)
            bestResult = result;
    }

    debug(bestResult.first);

    assert(!bestResult.second.empty());
    const auto action = bestResult.second.front();
    assert(state.canPress(action.i, action.j));
    bestResult.second.erase(bestResult.second.begin());

    return action;
}

NRPA::Result NRPA::nested(int level, Policy policy) {
    if (level == 0)
        return playout(*initialState, policy);

    Result bestResult = {-1, {}};
    for (int i = 0; i < ITERS /*&& timer.isTimeLeft()*/; ++i) {
        Result result = nested(level - 1, policy);
        if (result > bestResult)
            bestResult = result;
        adapt(policy, bestResult.second, ALPHA);
    }

    return bestResult;
}

NRPA::Result NRPA::playout(State state, const Policy& policy) {
    Result result;
    auto& sequence = result.second;

    while (!state.terminal()) {
        const auto move = moveWithPolicy(state, policy);
        state.apply(move);
        sequence.push_back(move);
    }

    result.first = state.realscore;
    return result;
}

Action NRPA::moveWithPolicy(const State& state, const Policy& policy) {
    const auto actions = state.getActions();
    prob_t sum = 0.0;
    for (const auto& move : actions)
        sum += std::exp(policy[code(move)]);

    prob_t t = Random::rand(sum);
    sum = 0.0;
    for (const auto& move : actions) {
        sum += std::exp(policy[code(move)]);
        if (t < sum)
            return move;
    }

    return actions.back();
}

void NRPA::adapt(Policy& policy, const Sequence& sequence, prob_t alpha) {
    auto state = *initialState;
    for (const auto& move : sequence) {
        const auto actions = state.getActions();
        prob_t sum = 0.0;
        for (const auto& m : actions)
            sum += std::exp(policy[code(m)]);

        for (const auto& m : actions) {
            int c = code(m);
            policy[c] -= alpha * std::exp(policy[c]) / sum;
        }
        int c = code(move);
        policy[c] += alpha;

        state.apply(move);
    }
}

NRPA::Policy NRPA::initialPolicy() const {
    Policy policy;
    for (int i = 0; i < MAX_ACTIONS; ++i)
        policy[i] = Random::rand<prob_t>();
    return policy;
}
