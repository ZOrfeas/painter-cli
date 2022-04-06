#ifndef LOG_HPP_
#define LOG_HPP_

#include <iostream>
#include <string>
#include <chrono>

#ifndef DEBUG
#define DEBUG true
#endif // DEBUG
#ifndef VERBOSE
#define VERBOSE true
#endif // VERBOSE

#if VERBOSE
    #define preamble_m(type) \
        make_logger_preamble_helper(type, __FILE__, __LINE__)
#else
    #define preamble_m(type) \
        make_logger_preamble_helper(type)
#endif // VERBOSE

#define log_m(msg) \
    log_print_helper(std::cout, preamble_m("LOG"), msg)
#define error_m(msg) \
    log_print_helper(std::cerr, preamble_m("ERROR"), msg); std::exit(1)
#if DEBUG
    #define debug_m(msg) \
        log_print_helper(std::cout, preamble_m("DEBUG"), msg)
#else
    #define debug_m(msg)
#endif // DEBUG
#define test_if(msg, cond) \
    if(cond) { log_print_helper(std::cout, preamble_m("TEST"),std::string("PASS: \t") + msg); } \
    else { log_print_helper(std::cout, preamble_m("TEST"),std::string("FAIL: \t") + msg); }
#define test_pred(msg, predicate) \
    test_if(msg, (predicate()))

static inline std::string getTime() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto in_time_t = system_clock::to_time_t(now);
    std::string t(std::ctime(&in_time_t));
    t.pop_back();
    return t;
}
static inline std::string make_logger_preamble_helper(const std::string& type) {
    return "[" + type + " " + getTime() + "]: ";
}
static inline std::string make_logger_preamble_helper(
    const std::string& type,
    const std::string& file,
    const int& line
) {
    return "[" + type + " " + getTime() + "]: line " + std::to_string(line) + " in " + file + "\t";
}
static inline void log_print_helper(
    std::ostream& target,
    const std::string& preamble,
    const std::string& message
) {
    target << preamble << message << std::endl;
}

// static inline void log_F(const std::string& message) {
//     log_print_helper(std::cout, make_logger_preamble("LOG"), message);
// }
// static inline void error_F(const std::string& message) {
//     log_print_helper(std::cerr, make_logger_preamble("ERROR"), message);
//     std::exit(1);
// }
// static inline void warning_F(const std::string& message) {
//     log_print_helper(std::cout, make_logger_preamble("WARNING"), message);
// }
// static inline void info_F(const std::string& message) {
//     log_print_helper(std::cout, make_logger_preamble("INFO"), message);
// }
// static inline void debug_F(const std::string& message) {
//     if (!DEBUG) return;
//     log_print_helper(std::cout, make_logger_preamble("DEBUG"), message);
// }

#endif // LOG_HPP_
