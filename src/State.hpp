#ifndef STATE_HPP
#define STATE_HPP

#include "Action.hpp"
#include "Common.hpp"
#include <iostream>
#include <unordered_set>

struct State {
	using color_t = short;
	static constexpr color_t EMPTY = -1;

	static constexpr int N = 15;
	static constexpr int DIRC = 4;
	static constexpr int dx[] = {0, 1, 0, -1};
	static constexpr int dy[] = {1, 0, -1, 0};
	static constexpr int MAX_NEIGHBORS = N * N / 2;

	color_t board[N][N];
	int score = 0;
	Action firstAction;

	bool terminal() const;
	bool canPress(const int& i, const int& j) const;
	bool inBounds(const int& i, const int& j) const;
	void apply(const Action& action);
	void propagate(const int left, const int right,
				   const int down, const int up);
	int propagateVertically(const int left, const int right,
							 const int down, const int up);
	void propagateHorizontally(const int left, const int right,
							   int emptyColumnsMask);
	int getNeighbors(std::unordered_set<State>& neighbors,
		bool firstLayer) const;

	#ifdef DEBUG
	bool isVerticallyCorrect() const;
	bool isHorizontallyCorrect() const;
	#endif

	friend std::ostream& operator<<(std::ostream& out,
		const State& state);
	bool operator>(const State& state) const;
	bool operator==(const State& state) const;
};

namespace std {
	template<>
	struct hash<State> {
		size_t operator()(const State& state) const {
			size_t hash = 0;
			for (int i = 0; i < state.N; ++i)
				for (int j = 0; j < state.N; ++j)
					hash = hash * 31 + (state.board[i][j] + 1);
			return hash * 31 + state.score;
		}
	};
};

#endif /* STATE_HPP */
