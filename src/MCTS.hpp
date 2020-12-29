#ifndef MCTS_HPP
#define MCTS_HPP

#include "Strategy.hpp"

class MCTS : public Strategy {
public:
    Action chooseAction(const State& game) override;
    void end() const override;

private:
};

#endif /* MCTS_HPP */