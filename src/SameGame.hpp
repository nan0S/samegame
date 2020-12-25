#ifndef SAME_GAME_HPP
#define SAME_GAME_HPP

#include "State.hpp"
#include "Action.hpp"

class SameGame {
public:
	static void init();
	static void run();

private:
	static Action chooseAction();

private:
	static State game;
  static int totalNextCount;
	static constexpr int BEAM_WIDTH = 100;
};

#endif /* SAME_GAME_HPP */
