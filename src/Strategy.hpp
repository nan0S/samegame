#ifndef STRATEGY_HPP
#define STRATEGY_HPP

#include "Action.hpp"
#include "State.hpp"

class Strategy {
public:
    virtual Action chooseAction(const State& game) = 0;
    virtual void end() const = 0;
    virtual ~Strategy() = default;

protected:
    State game;
};

#endif /* STRATEGY_HPP */
