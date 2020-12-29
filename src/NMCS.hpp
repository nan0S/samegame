#ifndef NMCS_HPP
#define NMCS_HPP

#include "Strategy.hpp"
#include <vector>

class NMCS : public Strategy {
public:
    using Sequence = State::Sequence;
    using Result = std::pair<int, Sequence>;

public:
    NMCS();
    void init(const State& game) override;
    Result nested(State state, int level);
    Result sample(State state);
    Action chooseAction(const State& game) override;
    void end() const override;

private:
    static constexpr int LEVEL = 2;
    Stoper stoper;
    std::vector<Action> bestSequence;
};

#endif /* MCTS_HPP */
