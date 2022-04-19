#include <command.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace painter::cli;

class Defaults {};

class CommandTest : public ::testing::Test {
    protected:
        static Defaults d;
        CommandTest() {}        
};
Defaults CommandTest::d{};