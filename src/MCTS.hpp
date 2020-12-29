#ifndef MCTS_HPP
#define MCTS_HPP

#include "Strategy.hpp"

class MCTS : public Strategy {
public:
    Action chooseAction(const State& state, float timeLimit) override; 
    void end() const override;
};
    
#endif /* MCTS_HPP */