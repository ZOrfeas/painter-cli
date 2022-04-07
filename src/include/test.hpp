#ifndef TEST_HPP_
#define TEST_HPP_

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <concepts>

#include <log.hpp>

template<typename T>
concept TestPredicate = requires(T t) {
    { t() } -> std::same_as<bool>;
};
template<typename T>
concept TestCond = requires(T t) {
    { (bool)t } -> std::same_as<bool>;
};

void test(const std::string& msg, TestCond auto cond) {
    std::string res;
    if ((bool)cond) 
        res = "(PASS)";
    else
        res = "(FAIL)";
    print_left_right_text(std::cout, msg, res, 80, '.');
}

// void test(const std::string& msg, TestPredicate auto pred, int indent = 0) {
//     std::string res;
//     if (pred()) 
//         res = "(PASS)";
//     else
//         res = "(FAIL)";
//     print_left_right_text(std::cout, msg, res, 100, '.');
//     // std::cout << std::left << std::setw(testtextwidth) << std::setfill('.') << 
//     //     format_log(preamble("TEST"), msg) << std::right << std::setfill('.') << res << std::endl;
// }

#define tsuite(msg, ...) do { \
        std::string text = std::string("Test suite: ") + msg; \
        print_centered_text(std::cout, "--", 80, '-'); \
        print_centered_text(std::cout, text, 80, '-'); \
        print_centered_text(std::cout, "--", 80, '-'); \
        __VA_ARGS__; \
        std::cout << '\n'; \
    } while (0)
#endif // TEST_HPP_