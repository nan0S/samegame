#include "Common.hpp"

Timer::Timer(time_t timeLimit) : timeLimit(timeLimit), start(clock_t::now()) {

}

bool Timer::isTimeLeft() const {
	auto now = clock_t::now();
	return std::chrono::duration<time_t>(now - start).count() * 1000 <= timeLimit;
}

Stoper::Stoper(time_t timeLimit) : timeLimit(timeLimit) {

}

void Stoper::start() {
    startTime = clock_t::now();
}

bool Stoper::isTimeLeft() const {
    auto now = clock_t::now();
    return std::chrono::duration<time_t>(now - startTime).count() * 1000 <= timeLimit;
}
