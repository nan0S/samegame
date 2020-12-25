#include "SameGame.hpp"

#include "Common.hpp"
#include <iostream>
#include <cassert>
#include <array>
#include <algorithm>

State SameGame::game;

void SameGame::init() {
	for (int i = State::N - 1; i >= 0; --i)
		for (int j = 0; j < State::N; ++j)
			std::cin >> game.board[j][i];
}

void SameGame::run() {
	debug(game);
	while (!game.terminal()) {
		Action action = chooseAction();
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
	debug(game.score);
}

#if 1
Action SameGame::chooseAction() {
	assert(!game.terminal());

	static constexpr int STATES = State::MAX_NEIGHBORS * BEAM_WIDTH;
	static std::array<State, STATES> beam;
	static std::unordered_set<State> next;
	next.clear();

	int beamSize = 1;
	beam[0] = game;

	int depth = 0;
	float timeLimit = 49;

	for (Timer timer(timeLimit); timer.isTimeLeft(); ++depth) {
		assert(beamSize > 0);
		for (int i = 0; i < beamSize && timer.isTimeLeft(); ++i) {
			const auto& state = beam[i];
			state.getNeighbors(next, depth == 0);
		}

		if (!timer.isTimeLeft())
			break;

		assert(!next.empty());
		beamSize = 0;
		for (const auto& state : next)
			beam[beamSize++] = state;

		int considerCount = std::min(beamSize, BEAM_WIDTH);
		std::partial_sort(
			beam.data(),
			beam.data() + considerCount,
			beam.data() + beamSize,
			std::greater<State>());
		for (int i = 0; i < considerCount - 1; ++i)
			assert(beam[i].score >= beam[i + 1].score);

		next.clear();
		beamSize = considerCount;
	}

	assert(beamSize > 0);
	const auto& action = beam[0].firstAction;
	assert(game.canPress(action.i, action.j));
	return action;
}
#endif

#if 0
// take first greedy
Action SameGame::chooseAction() {
	assert(!game.terminal());

	int best = -1;
	Action bestAction = {-1, -1};

	for (int i = 0; i < State::N; ++i)
		for (int j = 0; j < State::N; ++j)
			if (game.canPress(i, j)) {
				auto next = game;
				next.apply({i, j});
				if (next.score > best) {
					best = next.score;
					bestAction = {i, j};
				}
			}

	return bestAction;
}
#endif

#if 0
// take first greedy but with sorting
Action SameGame::chooseAction() {
	assert(!game.terminal());

	int best = -1;
	Action bestAction = {-1, -1};

	for (int i = 0; i < State::N; ++i)
		for (int j = 0; j < State::N; ++j)
			if (game.canPress(i, j)) {
				auto next = game;
				next.apply({i, j});
				if (next.score > best) {
					best = next.score;
					bestAction = {i, j};
				}
			}

	static constexpr int STATES = State::MAX_NEIGHBORS * BEAM_WIDTH;
	std::array<State, STATES> current, next;
	int currentCount = 1, nextCount = 0;
	current[0] = game;

	nextCount += current[0].getNeighbors(next.data() + nextCount, true);
	assert(nextCount > 0);

	std::stable_sort(next.data(), next.data() + nextCount, std::greater<State>());
	for (int i = 0; i < nextCount - 1; ++i)
		assert(next[i].score >= next[i + 1].score);

	for (int i = 0; i < nextCount; ++i) {
		const auto& action = next[i].firstAction;
		assert(game.canPress(action.i, action.j));
	}

	assert(bestAction == next[0].firstAction);

	return next[0].firstAction;
}
#endif

#if 0
// take first greedy but with sorting with unordered_set
Action SameGame::chooseAction() {
	assert(!game.terminal());

	int best = -1;
	Action bestAction = {-1, -1};

	for (int i = 0; i < State::N; ++i)
		for (int j = 0; j < State::N; ++j)
			if (game.canPress(i, j)) {
				auto next = game;
				next.apply({i, j});
				if (next.score > best) {
					best = next.score;
					bestAction = {i, j};
				}
			}

	static constexpr int STATES = State::MAX_NEIGHBORS * BEAM_WIDTH;
	std::array<State, STATES> beam;
	std::unordered_set<State> next;
	int beamSize = 1;
	beam[0] = game;

	int nextCount = beam[0].getNeighbors(next, true);
	assert(nextCount > 0);
	// not really
	assert(next.size() == nextCount);

	nextCount = 0;
	for (const auto& state : next)
		beam[nextCount++] = state;

	std::stable_sort(beam.data(), beam.data() + nextCount, std::greater<State>());
	for (int i = 0; i < nextCount - 1; ++i)
		assert(beam[i].score >= beam[i + 1].score);

	for (int i = 0; i < nextCount; ++i) {
		const auto& action = beam[i].firstAction;
		assert(game.canPress(action.i, action.j));
	}

	assert(best - game.score == beam[0].score - game.score);

	return beam[0].firstAction;
}
#endif
