#include <string_view>

#include <utils.hpp>
#include <gtest/gtest.h>

#include <flopt.hpp>

using namespace painter::cli;

class Defaults {
public:
    std::string_view const int_name = "int_flag";
    std::string_view const int_desc = "int";
    int const int_v1 = 42, int_v2 = 17;
    std::string_view const int_shorthand = "i";

    constexpr Defaults() {};

};

class TestFlopt : public ::testing::Test {
protected:
    static constexpr Defaults d{};
    
    static constexpr auto initializes_properly() {
        auto int_flag = Flopt<int>(
            d.int_name, d.int_desc, d.int_v1, d.int_shorthand
        );
        return int_flag.name == d.int_name &&
            int_flag.description == d.int_desc &&
            int_flag.shorthand == d.int_shorthand &&
            int_flag.is_flag == false &&
            int_flag.is_option == true &&
            int_flag.value == d.int_v1;
    }
};
TEST_F(TestFlopt, InitializesProperly) {
    CONSTEXPR_AND_EXPECT_TRUE(initializes_properly());
}