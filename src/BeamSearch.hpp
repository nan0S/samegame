#ifndef BEAM_SEACH_HPP
#define BEAM_SEACH_HPP

#include "Strategy.hpp"

class BeamSearch : public Strategy {
public:
    Action chooseAction(const State& game, float timeLimit) override;
    void end() const override;

private:
    static constexpr int BEAM_WIDTH = 100;
    int totalNextCount = 0;
};

#endif /* BEAM_SEARCH_HPP */
