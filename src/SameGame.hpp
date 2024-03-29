#ifndef SAME_GAME_HPP
#define SAME_GAME_HPP

#include "Strategy.hpp"
#include <cassert>

template<typename T>
class SameGame {
public:
	static void init() {
        game.init();
    }

	static void run() {
        strategy = new StrategyT();
        strategy->init(game);

        int roundCount = 0;
        while (!game.terminal()) {
            Action action = strategy->chooseAction(game,
                roundCount++ == 0 ? 19900 : 48);
            std::cout << action << std::endl;
            game.apply(action);
            debug(game);

            #ifndef LOCAL
            for (int i = State::N - 1; i >= 0; --i)
                for (int j = 0; j < State::N; ++j) {
                    State::color_t color;
                    std::cin >> color;
                    assert(game.board[j][i] == color);
                }
            #endif
        }

        debug(roundCount);

        strategy->end();
        delete strategy;
    }

private:
    using StrategyT = T;
    static State game;
    static Strategy* strategy;
};

template<typename T>
State SameGame<T>::game;
template<typename T>
Strategy* SameGame<T>::strategy;

#endif /* SAME_GAME_HPP */
