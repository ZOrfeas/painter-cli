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
        std::string intFlagName = "int_flag";
        std::string intFlagDescription = "An integer flag";
        int intFlagDefault = 42;
        std::string intFlagShorthand = "i";
        
        std::string floatFlagName = "float_flag";
        std::string floatFlagDescription = "A float flag";
        float floatFlagDefault = 3.14;
        std::string floatFlagShorthand = "f";

        std::string hostnameFlagName = "hostname_flag";
        std::string hostnameFlagDescription = "A hostname flag";
        Hostname hostnameFlagDefault = {"localhost", 80};
        std::string hostnameFlagShorthand = "h";

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
