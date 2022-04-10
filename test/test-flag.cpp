#include <iostream>
#include <string>

#include <gtest/gtest.h>
#include <flag.hpp>

using namespace pnt_cli;
using namespace std;

struct Hostname {
    std::string name;
    int port;
};
template<>
Hostname pnt_cli::fromString<Hostname>(const std::string& str) {
    auto colon_pos = str.find_first_of(':');
    if (colon_pos == std::string::npos) {
        error_m("Expected hostname:port");
    }
    return {
        .name = str.substr(0, colon_pos),
        .port = std::stoi(str.substr(colon_pos + 1))
    };
}
template<>
std::string pnt_cli::toString<Hostname>(const Hostname& val) {
    return val.name + ":" + std::to_string(val.port);
}

class FlagSetTest : public ::testing::Test {
    protected:
        FlagSet fs;
        static const std::string intFlagName;
        static const std::string intFlagDescription;
        static const int intFlagDefault;
        static const std::string intFlagShorthand;
        
        static const std::string floatFlagName;
        static const std::string floatFlagDescription;
        static const float floatFlagDefault;
        static const std::string floatFlagShorthand;

        static const std::string hostnameFlagName;
        static const std::string hostnameFlagDescription;
        static const Hostname hostnameFlagDefault;
        static const std::string hostnameFlagShorthand;

        void addIntFlag() {
            fs.addFlag<int>(intFlagName, intFlagDescription, intFlagDefault, intFlagShorthand);
        }
        void addFloatFlag() {
            fs.addFlag<float>(floatFlagName, floatFlagDescription, floatFlagDefault, floatFlagShorthand);
        }
        void addHostnameFlag() {
            fs.addFlag<Hostname>(hostnameFlagName, hostnameFlagDescription, hostnameFlagDefault, hostnameFlagShorthand);
        }
        void addAllFlags() {
            addIntFlag();
            addFloatFlag();
            addHostnameFlag();
        }        
};
const std::string FlagSetTest::intFlagName = "int_flag";
const std::string FlagSetTest::intFlagDescription = "An integer flag";
const int FlagSetTest::intFlagDefault = 42;
const std::string FlagSetTest::intFlagShorthand = "i";
const std::string FlagSetTest::floatFlagName = "float_flag";
const std::string FlagSetTest::floatFlagDescription = "A float flag";
const float FlagSetTest::floatFlagDefault = 3.14;
const std::string FlagSetTest::floatFlagShorthand = "f";
const std::string FlagSetTest::hostnameFlagName = "hostname_flag";
const std::string FlagSetTest::hostnameFlagDescription = "A hostname flag";
const Hostname FlagSetTest::hostnameFlagDefault = {"localhost", 80};
const std::string FlagSetTest::hostnameFlagShorthand = "h";

TEST_F(FlagSetTest, InitializesProperly) {
    EXPECT_TRUE(fs.empty());
}
TEST_F(FlagSetTest, SimpleFlagOperationsWork) {
    addIntFlag();
    addFloatFlag();
    EXPECT_FALSE(fs.empty());
    EXPECT_EQ(fs.size(), 2);
    EXPECT_TRUE(fs.find<int>(intFlagName));
    EXPECT_TRUE(fs.find<float>(floatFlagShorthand));
    EXPECT_FALSE(fs.find<float>(intFlagName));
    EXPECT_EQ(fs.get<int>(intFlagShorthand), intFlagDefault);
    EXPECT_TRUE(fs.set<int>(intFlagShorthand, "10"));
    EXPECT_EQ(fs.get<int>(intFlagShorthand), 10);    
}
TEST_F(FlagSetTest, CustomFlagTypesWork) {
    addHostnameFlag();
    EXPECT_TRUE(fs.size() == 1);
    EXPECT_TRUE(fs.find<Hostname>(hostnameFlagName));
    EXPECT_EQ(fs.get<Hostname>(hostnameFlagShorthand)->name, hostnameFlagDefault.name);
    EXPECT_EQ(fs.get<Hostname>(hostnameFlagShorthand)->port, hostnameFlagDefault.port);
    EXPECT_TRUE(fs.set<Hostname>(hostnameFlagShorthand, "localhost:8080"));
    EXPECT_EQ(fs.get<Hostname>(hostnameFlagShorthand)->name, "localhost");
    EXPECT_EQ(fs.get<Hostname>(hostnameFlagShorthand)->port, 8080);
}
