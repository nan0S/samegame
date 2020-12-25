#include "State.hpp"

#include <cassert>
#include <cstring>
#include <queue>
#include <algorithm>

bool State::terminal() const {
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			if (canPress(i, j))
				return false;
	return true;
}

bool State::canPress(const int& i, const int& j) const {
	assert(inBounds(i, j));
	if (board[i][j] == EMPTY)
		return false;
	for (int k = 0; k < DIRC; ++k) {
		int ni = i + dx[k];
		int nj = j + dy[k];
		if (!inBounds(ni, nj))
			continue;
		if (board[ni][nj] == board[i][j])
			return true;
	}
	return false;
}

bool State::inBounds(const int& i, const int& j) const {
	return 0 <= i && i < N && 0 <= j && j < N;
}

void State::apply(const Action& action) {
	const auto& [i, j] = action;
	assert(canPress(i, j));

	static bool enqueued[N][N];
	std::memset(enqueued, 0, sizeof(enqueued));

	std::queue<Action> q;
	q.push(action);
	enqueued[i][j] = true;

	int count = 0;

	while (!q.empty()) {
		const auto [i, j] = q.front();
		q.pop();

		const auto color = board[i][j];
		for (int k = 0; k < DIRC; ++k) {
			int ni = i + dx[k];
			int nj = j + dy[k];
			if (!inBounds(ni, nj) || enqueued[ni][nj])
				continue;
			if (color == board[ni][nj]) {
				q.push({ni, nj});
				enqueued[ni][nj] = true;
			}
		}

		board[i][j] = EMPTY;
		++count;
	}

	assert(count > 1);
	score += (count - 2) * (count - 2);

	propagate();

	// empty reward
	if (terminal())
		score += 1000;
}

void State::propagate() {
	propagateVertically();
	propagateHorizontally();
}

void State::propagateVertically() {
	for (int j = 0; j < N; ++j) {
		color_t* column = board[j];
		for (int i = 0, it = 1; i < N - 1; ++i) {
			if (column[i] != EMPTY)
				continue;
			if (it <= i)
				it = i + 1;
			while (it < N && column[it] == EMPTY)
				++it;
			assert(it <= N);
			if (it == N)
				break;

			assert(column[i] == EMPTY &&
				column[it] != EMPTY && 
				i < it);
			std::swap(column[i], column[it]);
		}
	}

	#ifdef DEBUG
	assert(isVerticallyCorrect());
	#endif
}

void State::propagateHorizontally() {
	auto isEmpty = [this](const int& j){
		return std::all_of(board[j], board[j] + N, [](const color_t& c){
			return c == EMPTY;
		});
	};

	for (int j = 0, jt = 1; j < N - 1; ++j) {
		if (!isEmpty(j))
			continue;
		if (jt <= j)
			jt = j + 1;
		while (jt < N && isEmpty(jt))
			++jt;
		assert(jt <= N);
		if (jt == N)
			break;

		assert(isEmpty(j) && 
			!isEmpty(jt) &&
			j < jt);

		for (int i = 0; i < N; ++i)
			std::swap(board[j][i], board[jt][i]);
	}

	#ifdef DEBUG
	assert(isHorizontallyCorrect());
	#endif
}

int State::getNeighbors(std::unordered_set<State>& neighbors, bool firstLayer) const {
	static bool visited[N][N];
	std::memset(visited, 0, sizeof(visited));

	auto floodfill = [this](const int& i, const int& j){
		assert(!visited[i][j]);
		assert(canPress(i, j));

		std::queue<Action> q;
		q.push({i, j});
		visited[i][j] = true;
		const auto color = board[i][j];

		while (!q.empty()) {
			const auto [i, j] = q.front();
			q.pop();

			assert(color == board[i][j]);
			assert(visited[i][j]);

			for (int k = 0; k < DIRC; ++k) {
				int ni = i + dx[k];
				int nj = j + dy[k];
				if (!inBounds(ni, nj) || visited[ni][nj])
					continue;
				if (color != board[ni][nj])
					continue;

				q.push({ni, nj});
				visited[ni][nj] = true;
			}
		}
	};

	int neighborCount = 0;
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			if (!visited[i][j] && canPress(i, j)) {
				floodfill(i, j);

				auto neighbor = *this;
				neighbor.apply({i, j});
				if (firstLayer)
					neighbor.firstAction = {i, j};

				auto [it, inserted] = neighbors.insert(neighbor);
				neighborCount += inserted;
			}

	if (neighborCount == 0)
		neighborCount = neighbors.insert(*this).second;

	return neighborCount;
}

#ifdef DEBUG
bool State::isVerticallyCorrect() const {
	for (int j = 0; j < N; ++j) {
		const color_t* column = board[j];
		bool metNonEmpty = false;
		for (int i = N - 1; i >= 0; --i) {
			if (column[i] != EMPTY)
				metNonEmpty = true;
			else if (metNonEmpty)
				return false;
		}
	}
	return true;
}

bool State::isHorizontallyCorrect() const {
	auto isEmpty = [this](const int& j){
		return std::all_of(board[j], board[j] + N, [](const color_t& c){
			return c == EMPTY;
		});
	};

	bool metNonEmpty = false;
	for (int j = N - 1; j >= 0; --j) {
		if (!isEmpty(j))
			metNonEmpty = true;
		else if (metNonEmpty)
			return false;
	}
	return true;
}
#endif

std::ostream& operator<<(std::ostream& out, const State& state) {
	static const char* colors[] = {
		"\033[0m",  /* RESET */
		"\033[31m", /* RED */
		"\033[32m", /* GREEN */
		"\033[34m", /* BLUE */
		"\033[33m", /* YELLOW */
		"\033[35m", /* PURPLE */
	};
	static const char* block = "\u25A0";

	out << "\n\t  ";
	for (int i = 0; i < state.N; ++i)
		out << i % 10 << " ";
	out << '\n';
	for (int i = state.N - 1; i >= 0; --i) {
		out << '\t' << i % 10 << " ";
		for (int j = 0; j < state.N; ++j) {
			const auto& c = state.board[j][i];
			out << colors[c + 1] << block << " " <<  colors[0];
		}
		out << "\n";
	}
	return out;
}

bool State::operator>(const State& state) const {
	return score > state.score;
}

bool State::operator==(const State& state) const {
	if (score != state.score)
		return false;
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			if (board[i][j] != state.board[i][j])
				return false;
	return true;
}
