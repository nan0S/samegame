#include "Common.hpp"

Timer::Timer(time_t timeLimit) : timeLimit(timeLimit), start(clock_t::now()) {

}

void Timer::set(float timeLimit) {
    this->timeLimit = timeLimit;
}

bool Timer::isTimeLeft() const {
	auto now = clock_t::now();
	return std::chrono::duration<time_t>(now - start).count() * 1000 <= timeLimit;
}

void Timer::reset() {
    start = clock_t::now();
}

std::mt19937 Random::rng(std::random_device{}());
