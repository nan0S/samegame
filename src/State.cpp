#include "State.hpp"

#include <cassert>
#include <cstring>
#include <queue>
#include <algorithm>

bool Info::operator>(const Info& info) const {
	return score > info.score;
}

std::ostream& operator<<(std::ostream& out, const Info& info) {
	return out << "score=" << info.score << ", action=" << info.action;
}

void State::init() {
	for (int i = State::N - 1; i >= 0; --i)
		for (int j = 0; j < State::N; ++j)
			std::cin >> board[j][i];
}

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
	if (!canPress(i, j)) {
		debug(*this);
		debug(action);
	}
	assert(canPress(i, j));

	static bool enqueued[N][N];
	std::memset(enqueued, 0, sizeof(enqueued));

	std::queue<Action> q;
	q.push(action);
	enqueued[i][j] = true;

	int count = 0;
	bool tabooColor = board[i][j] == getTaboo();
	int left = i, right = i;
	int down = j, up = j;

	while (!q.empty()) {
		const auto [i, j] = q.front();
		q.pop();

		if (i < left) left = i;
		else if (i > right) right = i;
		if (j < down) down = j;
		else if (j > up) up = j;

		assert(left <= i && i <= right);
		assert(down <= j && j <= up);

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
	realscore += (count - 2) * (count - 2);
	if (tabooColor)
		score -= 10000;

	propagate(left, right, down, up);

	// empty reward
	if (empty()) {
		score += 1000;
		realscore += 1000;
	}
}

void State::propagate(const int left, const int right,
					  const int down, const int up) {
	int emptyColumnsMask = propagateVertically(left, right, down, up);
	propagateHorizontally(left, right, emptyColumnsMask);
}

int State::propagateVertically(const int left, const int right,
								const int down, const int up) {
	assert(N <= sizeof(int) * 8);
	assert(left <= right);
	assert(0 <= left && left < N);
	assert(0 <= right && right < N);
	assert(down <= up);
	assert(0 <= down && down < N);
	assert(0 <= up && up < N);

	int emptyColumnsMask = down != 0 ?
		0 : ((1 << (right + 1)) - 1) ^ ((1 << left) -  1);

	if (down == 0) {
		for (int i = 0; i < N; ++i)
			if (left <= i && i <= right)
				assert(emptyColumnsMask & 1 << i);
			else
				assert(!(emptyColumnsMask & 1 << i));
	}

	for (int j = left; j <= right; ++j) {
		color_t* column = board[j];
		for (int i = down, it = down + 1; i < N - 1; ++i) {
			if (column[i] != EMPTY) {
				emptyColumnsMask &= ~(1 << j);
				continue;
			}
			if (it <= i)
				it = i + 1;
			while (it < N && column[it] == EMPTY)
				++it;
			assert(it <= N);
			if (it == N)
				break;
			emptyColumnsMask &= ~(1 << j);
			assert(column[i] == EMPTY &&
				column[it] != EMPTY && 
				i < it);
			std::swap(column[i], column[it]);
		}
	}

	#ifdef DEBUG
	assert(isVerticallyCorrect());
	#endif

	return emptyColumnsMask;
}

void State::propagateHorizontally(const int left, const int right,
								  int emptyColumnsMask) {
	assert(left <= right);
	assert(0 <= left && left < N);
	assert(0 <= right && right < N);

	auto isEmpty = [&emptyColumnsMask](const int& j){
		return emptyColumnsMask & 1 << j;
	};

	for (int i = left; i <= right; ++i)
		if (isEmpty(i))
			assert(emptyColumnsMask & 1 << i);
		else
			assert(!(emptyColumnsMask & 1 << i));

	for (int j = left, jt = left + 1; j < N - 1; ++j) {
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
		emptyColumnsMask ^= 1 << j;
		emptyColumnsMask ^= 1 << jt;
	}

	#ifdef DEBUG
	assert(isHorizontallyCorrect());
	#endif
}

bool State::empty() const {
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			if (board[i][j] != EMPTY)
				return false;
	return true;
}

int State::getNeighbors(Info* infos) const {
	static bool visited[N][N];
	std::memset(visited, 0, sizeof(visited));

	auto floodfill = [this](const int& i, const int& j){
		assert(!visited[i][j]);
		assert(canPress(i, j));

		std::queue<Action> q;
		q.push({i, j});
		visited[i][j] = true;
		int count = 0;
		const auto color = board[i][j];

		while (!q.empty()) {
			const auto [i, j] = q.front();
			q.pop();
			++count;

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

		return count;
	};

	int neighborCount = 0;
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			if (!visited[i][j] && canPress(i, j)) {
				int count = floodfill(i, j);
				assert(count > 1);

				auto& info = infos[neighborCount++];
				info.action = {i, j};
				info.score = score + (count - 2) * (count - 2);
				info.state = this;
			}

	if (neighborCount == 0)
		infos[neighborCount++] = { {-1, -1}, score, this };

	return neighborCount;
}

State::color_t State::getTaboo() const {
	static int colorCount[5];
	std::fill(colorCount, colorCount + 5, 0);
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j) {
			const auto c = board[i][j];
			if (c != EMPTY) {
				assert(0 <= c && c < 5);
				++colorCount[c];
			}
		}
	return std::max_element(colorCount, colorCount + 5) - colorCount;
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
	out << "score: " << state.score << "\n";
	out << "realscore: " << state.realscore << "\n";
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
