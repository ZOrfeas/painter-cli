#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <string>
#include <chrono>

namespace painter::utils {

static inline std::string get_now_string() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto in_time_t = system_clock::to_time_t(now);
    std::string t(std::ctime(&in_time_t));
    t.pop_back();
    return t;
}

}

#endif // UTILS_HPP_