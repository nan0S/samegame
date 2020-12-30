#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include "Action.hpp"
#include "State.hpp"

class Strategy {
public:
    virtual void init(const State& game);
    virtual Action chooseAction(const State& game, float timeLimit) = 0;
    virtual void end() const;
    virtual ~Strategy() = default;
};

#endif /* STRATEGY_HPP */
