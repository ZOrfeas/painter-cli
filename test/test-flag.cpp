#include <memory>
#include <flag.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace painter::cli;

class Defaults {
    public:
        string get_name() { return "flag"; }
        string get_description() { return "this is a test flag"; }
        bool get_value() { return false; }
        string get_shorthand() { return "f"; }
};

class FlagTest : public ::testing::Test {
    protected:
        static Defaults d;
        FlagTest() {
            flag = make_shared<Flag>(
                "flag",
                "this is a test flag",
                false,
                "f"
            );
        }
        shared_ptr<Flag> flag;
};
Defaults FlagTest::d{};

TEST_F(FlagTest, InitializesProperly) {
    EXPECT_EQ(flag->get(), d.get_value());
}
TEST_F(FlagTest, TogglingWorks) {
    flag->set();
    EXPECT_EQ(flag->get(), !d.get_value());
    flag->set();
    EXPECT_EQ(flag->get(), d.get_value());
}