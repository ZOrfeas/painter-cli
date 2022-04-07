#include <iostream>
#include <string>

#include <test.hpp>
#include <flag.hpp>

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


int main() {
    using namespace pnt_cli;
    using namespace std;
    static_assert(FlagType<int>);
    static_assert(FlagType<float>);
    static_assert(FlagType<std::string>);
    static_assert(FlagType<bool>);
    static_assert(FlagType<Hostname>);
    TestSuite tsuite("flag.hpp tests");

    FlagSet fs;
    tsuite.createGroup("Basic FlagSet initialization checks")
        .t("Freshly constructed Flagset should be empty", fs.empty());
    
    fs.addFlag<int>("t_int", "int flag", 1, "i");
    fs.addFlag<float>( "t_float", "float flag", 1.0f, "f");

    tsuite.createGroup("Basic flag operations")
        .t("After insertion of two flags, flagset should not be empty", !fs.empty())
        .t("Flagset should have 2 flags", fs.size() == 2)
        .t("Check if first flag can be found", fs.find<int>("t_int"))
        .t("Check if first flag can be found via shorthand", fs.find<int>("i"))
        .t("Check if first flag's value is the default", (*fs.get<int>("t_int")) == 1)
        .t("Check if first flag can be set", fs.set<int>("t_int", "2"))
        .t("Check if first flag value changed after setting", (*fs.get<int>("t_int")) == 2);

    fs.addFlag<Hostname>("test_str", "test custom flag", fromString<Hostname>("localhost:5000"), "s");
    tsuite.createGroup("Custom flag type")
        .t("After insertion of third (custom flag) flagset should now have 3 flags", fs.size() == 3)
        .t("Check if custom flag can be found", fs.get<Hostname>("test_str"))
        .t("Check if custom flag has the default value", [&]() {
            Hostname tmp = *fs.get<Hostname>("test_str");
            std::cout << tmp.name << ":" << tmp.port << std::endl;
            return tmp.name == "localhost" && tmp.port == 5000;
        }()) //!Note, if provided as a callback it fails due to running after the set-test
        .t("Check if custom flag can be set", fs.set<Hostname>("test_str", "localhost:5001"))
        .t("Check if custom flag value changed after setting", [&]() {
            Hostname tmp = *fs.get<Hostname>("test_str");
            return tmp.name == "localhost" && tmp.port == 5001;
        });

    return tsuite.run();
}