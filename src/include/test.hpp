#ifndef TEST_HPP_
#define TEST_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <concepts>
#include <utility>

#include <log.hpp>

template<typename T>
concept TestCond = requires(T t) {
    { (bool)t } -> std::same_as<bool>;
};

static inline void test(const std::string& msg, TestCond auto cond) {
    std::string res;
    if ((bool)cond) 
        res = "(PASS)";
    else
        res = "(FAIL)";
    print_left_right_text(std::cout, msg, res, 80, '.');
}
static inline void tsuite(const std::string& msg, std::vector<std::pair<std::string, bool>> tests) {
    std::string intro = std::string("Test suite: ") + msg;
    print_centered_text(std::cout, intro, 80, '-');
    print_centered_text(std::cout, "--", 80, '-');
    for (const auto &[msg, cond] : tests) {
        test(msg, cond);
    }
    std::cout << '\n';
}

#endif // TEST_HPP_
