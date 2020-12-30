#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <chrono>
#include <random>

#if defined(LOCAL) && !defined(NDEBUG)
#define DEBUG
#endif

#ifdef DEBUG

inline void __debug(const char* s, const char*) {
    std::cerr << s << ": ";
}

template<typename T>
void __debug(const char* s, const T& x) {
    std::cerr << s << ": " << x << " ";
}

template<typename T, typename... Args>
void __debug(const char* s, const T& x, const Args&... rest) {
    int bracket = 0;
    char c;
    while ((c = *s) != ',' || bracket)
    {
        std::cerr << *s++;
        switch (c)
        {
            case '(':
            case '{':
            case '[':
                ++bracket;
                break;
            case ')':
            case '}':
            case ']':
                --bracket;
        }
    }
    std::cerr << ": ";
    std::cerr << x << ",";
    __debug(s + 1, rest...);

}

template<typename... Args>
void _debug(const char* s, const Args&... rest) {
    __debug(s, rest...);
    std::cerr << std::endl;
}

#define debug(...) _debug(#__VA_ARGS__, __VA_ARGS__)
#else
#define debug(...)
#endif

class Timer {
public:
    using time_t = float;
    using clock_t = std::chrono::high_resolution_clock;

    Timer() = default;
    Timer(time_t timeLimit);
    void set(float timeLimit);
    bool isTimeLeft() const;
    void reset();

private:
    time_t timeLimit;
    clock_t::time_point start;
};

namespace Random {
    extern std::mt19937 rng;

    template<typename T>
    using T_Int = std::enable_if_t<
        std::is_integral_v<T>, T>;

    template<typename T>
    using NT_Int = std::enable_if_t<
        !std::is_integral_v<T>, T>;

    template<typename T>
    using dist_t = std::conditional_t<
        std::is_integral_v<T>,
        std::uniform_int_distribution<T>,
        std::uniform_real_distribution<T>>;

    template<typename T>
    NT_Int<T> rand() {
        return dist_t<T>{0, 1}(rng);
    }

    template<typename T>
    NT_Int<T> rand(T x) {
        return dist_t<T>{0, x}(rng);
    }
}

#endif /* COMMON_HPP */
