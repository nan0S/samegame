#ifndef STATE_HPP
#define STATE_HPP

#include "Action.hpp"
#include "Common.hpp"
#include <iostream>
#include <unordered_set>
#include <vector>

struct State;

struct Info {
	Action action;
	int score;
	const State* state;

	bool operator>(const Info& info) const;
	friend std::ostream& operator<<(std::ostream& out, const Info& info);
};

struct State {
	using color_t = short;
	using Sequence = std::vector<Action>;
	static constexpr color_t EMPTY = -1;

	static constexpr int N = 15;
	static constexpr int DIRC = 4;
	static constexpr int dx[DIRC] = {0, 1, 0, -1};
	static constexpr int dy[DIRC] = {1, 0, -1, 0};
	static constexpr int MAX_NEIGHBORS = N * N / 2;

	color_t board[N][N];
	int score = 0;
	int realscore = 0;
	Action firstAction;

	void init();
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
	bool empty() const;
	int getNeighbors(Info* infos) const;
	int floodfill(const int i, const int j, bool visited[N][N]) const;
	color_t getTaboo() const;
	Sequence getActions() const;

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
