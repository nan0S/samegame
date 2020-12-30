#ifndef NMCS_HPP
#define NMCS_HPP

#include "Strategy.hpp"
#include <vector>

class NMCS : public Strategy {
public:
    using Sequence = State::Sequence;
    using Result = std::pair<int, Sequence>;

public:
    Result nested(State state, int level);
    Result sample(State state);
    Action chooseAction(const State& game, float timeLimit) override;

private:
    static constexpr int LEVEL = 2;
    Timer timer;
    Result bestResult = {-1, {}};
};

#endif /* MCTS_HPP */
