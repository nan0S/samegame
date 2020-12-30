#ifndef NRPA_HPP
#define NRPA_HPP

#include "Strategy.hpp"

class NRPA : public Strategy {
public:
    static constexpr int MAX_ACTIONS = State::N * State::N;

    using prob_t = double;
    using Sequence = State::Sequence;
    using Result = std::pair<int, Sequence>;
    using Policy = std::array<prob_t, MAX_ACTIONS>;

public:
    Action chooseAction(const State& state, float timeLimit) override;
    Result nested(int level, Policy policy);
    Result playout(State state, const Policy& policy);
    inline int code(const Action& action);
    Action moveWithPolicy(const State& state, const Policy& policy);
    void adapt(Policy& policy, const Sequence& sequence, prob_t alpha);
    Policy initialPolicy() const;

private:
    static constexpr int LEVEL = 4;
    static constexpr int ITERS = 10;
    static constexpr float ALPHA = 1.0;
    Timer timer;
    Result bestResult;
    const State* initialState;
};

int NRPA::code(const Action& action) {
    return action.i * State::N + action.j;
}
    
#endif /* NRPA_HPP */
