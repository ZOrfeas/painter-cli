#include <flopt_set.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace painter::cli;

class Defaults {
    public:
        
};

class FloptSetTest : public ::testing::Test {
    protected:
        static Defaults d;

        FloptSet fs;
};
Defaults FloptSetTest::d{};

TEST_F(FloptSetTest, InitializesProperly) {
    EXPECT_EQ(fs.size(), 0);
    EXPECT_TRUE(fs.is_empty());
}