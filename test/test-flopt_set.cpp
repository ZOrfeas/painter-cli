#include <flopt_set.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace painter::cli;

class Defaults {
    public:
        string get_flag_name() { return "flag"; }
        string get_flag_description() { return "this is a test flag"; }
        bool get_flag_value() { return false; }
        string get_flag_shorthand() { return "f"; }
        string get_option_name() { return "option"; }
        string get_option_description() { return "this is a test option"; }
        template<OptionType T> T get_option_value();
        string get_option_shorthand() { return "o"; }        
};
template<> int Defaults::get_option_value<int>() { return 42; }
template<> float Defaults::get_option_value<float>() { return 3.14; }
template<> std::string Defaults::get_option_value<std::string>() { return "hello"; }

class FloptSetTest : public ::testing::Test {
    protected:
        static Defaults d;
        void add_flag() {
            fs.add<bool>(
                d.get_flag_name(),
                d.get_flag_description(),
                d.get_flag_value(),
                d.get_flag_shorthand()
            );
        }
        template<OptionType T>
        void add_option() {
            fs.add<T>(
                d.get_option_name(),
                d.get_option_description(),
                d.get_option_value<T>(),
                d.get_option_shorthand()
            );
        }
        FloptSet fs;
};
Defaults FloptSetTest::d{};

TEST_F(FloptSetTest, InitializesProperly) {
    EXPECT_EQ(fs.size(), 0);
    EXPECT_TRUE(fs.is_empty());
}
TEST_F(FloptSetTest, AddingFlagsWorks) {
    add_flag();
    EXPECT_EQ(fs.size(), 1);
    EXPECT_FALSE(fs.is_empty());
}
TEST_F(FloptSetTest, AddingOptionsWorks) {
    add_option<int>();
    EXPECT_EQ(fs.size(), 1);
    EXPECT_FALSE(fs.is_empty());
}
TEST_F(FloptSetTest, NameCollisionsAreDetected) {
    add_flag();
    EXPECT_DEATH(add_flag(), "Flopt .* already exists");
}
TEST_F(FloptSetTest, NameCollisionsAreDetected2) {
    add_option<int>();
    EXPECT_DEATH(add_option<float>(), "Flopt .* already exists");
}
TEST_F(FloptSetTest, ValueIsAccessible) {
    add_flag();
    EXPECT_EQ(fs.get_value<bool>(d.get_flag_name()), d.get_flag_value());
    add_option<string>();
    EXPECT_EQ(fs.get_value<string>(d.get_option_name()), d.get_option_value<string>());
}
TEST_F(FloptSetTest, ValueIsSettable) {
    add_flag();
    EXPECT_EQ(fs.get_value<bool>(d.get_flag_name()), d.get_flag_value());
    fs.set_value(d.get_flag_name(), "");
    EXPECT_EQ(fs.get_value<bool>(d.get_flag_name()), !d.get_flag_value());
    add_option<int>();
    EXPECT_EQ(fs.get_value<int>(d.get_option_name()), d.get_option_value<int>());
    fs.set_value(d.get_option_name(), "8");
    EXPECT_EQ(fs.get_value<int>(d.get_option_name()), 8);
}