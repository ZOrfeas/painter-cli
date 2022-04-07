#ifndef TEST_HPP_
#define TEST_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <concepts>
#include <utility>
#include <functional>

#include <log.hpp>

template<typename T>
concept TestPredC = requires(T t) {
    { t() } -> std::same_as<bool>;
};
template<typename T>
concept TestCondC = requires(T t) {
    { (bool)t } -> std::same_as<bool>;
};

class Test {
    public:
    std::string name;
    Test(std::string name) : name(name) {}
    virtual bool run() const = 0;
    virtual ~Test() {}
};
class TestPred : public Test {
    std::function<bool()> pred;
    public:
    TestPred(std::string name, std::function<bool()> pred) : Test(name), pred(pred) {}
    bool run() const override {
        return pred();
    }
};
class TestCond : public Test {
    bool cond;
    public:
    TestCond(std::string name, bool cond) : Test(name), cond(cond) {}
    bool run() const override {
        return cond;
    }
};

class TestGroup {
    private:
        std::string name;
        std::vector<Test*> tests;
    public:
        TestGroup(std::string name) : name(name) {}
        template<TestPredC T>
        TestGroup& t(std::string name, T pred) {
            tests.push_back(new TestPred(name, [pred]() {return pred();}));
            return *this;
        }
        template<TestCondC T>
        TestGroup& t(std::string name, T cond) {
            tests.push_back(new TestCond(name, (bool)cond));
            return *this;
        }
        bool run() const {
            bool success = true;
            print_centered_text(
                std::cout, std::string("Test group: ") + name,
                100, '-'
            );
            for (auto test : tests) {
                auto testRes = test->run();
                print_left_right_text(
                    std::cout, test->name,
                    testRes ? "(PASS)" : "(FAIL)",
                    80, '.'
                );
                success = success && testRes;
            }
            return success;
        }
};
class TestSuite {
    private:
        std::string name;
        std::vector<TestGroup> groups;
    public:
        TestSuite(const std::string& name) 
            : name(name) {}
        TestGroup& createGroup(const std::string& name) {
            groups.push_back(TestGroup(name));
            return groups.back();
        }

        bool run() const {
            bool success = true;
            print_centered_text(
                std::cout, std::string("Test suite: ") + name,
                120, '='
            );
            for (const auto& group : groups) {
                success = success && group.run();
            }
            //!Note: it's negated to return 0 on success
            return !success;
        }
};



// static inline void test(const std::string& msg, const TestResult& cond) {
//     std::string res;
//     if (cond) 
//         res = "(PASS)";
//     else
//         res = "(FAIL)";
//     print_left_right_text(std::cout, msg, res, 80, '.');
// }
// static inline void tsuite(const std::string& msg, std::vector<std::pair<std::string, TestResult>> tests) {
//     std::string intro = std::string("Test suite: ") + msg;
//     print_centered_text(std::cout, intro, 80, '-');
//     print_centered_text(std::cout, "--", 80, '-');
//     for (const auto &[msg, cond] : tests) {
//         test(msg, cond);
//     }
//     std::cout << '\n';
// }

#endif // TEST_HPP_
