#ifndef LOG_HPP_
#define LOG_HPP_

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>

static inline std::string getTime() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto in_time_t = system_clock::to_time_t(now);
    std::string t(std::ctime(&in_time_t));
    t.pop_back();
    return t;
}
static inline std::string preamble(const std::string& type) {
    return "[" + type + " " + getTime() + "]: ";
}
static inline std::string format_log(
    const std::string& log_preamble,
    const std::string& message
) {
    return log_preamble + message;
    // target << preamble << message << std::endl;
}
static inline void log_m(const std::string& msg) {
    std::cout << format_log(preamble("LOG"), msg);
}
static inline void error_m(const std::string& msg) {
    std::cerr << format_log(preamble("ERROR"), msg);
    std::exit(1);
}
static inline void print_centered_text(
    std::ostream& target,
    const std::string& text,
    int width = 80,
    char fill = ' '
) {
    target << std::right << std::setfill(fill) << std::setw(width/2) <<
        text.substr(0, text.length()/2) << std::left << std::setfill(fill) <<
        std::setw(width/2) << text.substr(text.length()/2) << std::endl;
}

static inline void print_left_right_text(
    std::ostream& target,
    const std::string& left,
    const std::string& right,
    int width = 80,
    char fill = ' '
) {
    target << std::left << std::setfill(fill) << std::setw(width - right.length()) <<
        left << std::right << std::setfill(fill) << std::setw(right.length()) <<
        right << std::endl;
}

#endif // LOG_HPP_
