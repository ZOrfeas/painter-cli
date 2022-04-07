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

    FlagSet fs;
    tsuite("Basic FlagSet initialization checks",{
        {"Freshly constructed Flagset should be empty", fs.empty()},
    });
    
    fs.addFlag<int>("t_int", "int flag", 1, "i");
    fs.addFlag<float>( "t_float", "float flag", 1.0f, "f");

    tsuite("Basic flag operations",{
        {"After insertion of two flags, flagset should not be empty", !fs.empty()},
        {"Flagset should have 2 flags", fs.size() == 2},

        {"Check if first flag can be found", !!fs.find<int>("t_int")},
        {"Check if first flag can be found via shorthand", !!fs.find<int>("i")},
        {"Check if first flag's value is the default", (*fs.get<int>("t_int")) == 1},
        {"Check if first flag can be set", fs.set<int>("t_int", "2")},
        {"Check if first flag value changed after setting", (*fs.get<int>("t_int")) == 2},
    });

    fs.addFlag<Hostname>("test_str", "test custom flag", fromString<Hostname>("localhost:5000"), "s");
    tsuite("Custom flag type", {
        {"After insertion of third (custom flag) flagset should now have 3 flags", fs.size() == 3},
        {"Check if custom flag can be found", !!fs.get<Hostname>("test_str")},
        {"Check if custom flag has the default value", [&]() {
            Hostname tmp = *fs.get<Hostname>("test_str");
            return tmp.name == "localhost" && tmp.port == 5000;
        }()},
        {"Check if custom flag can be set", fs.set<Hostname>("test_str", "localhost:5001")},
        {"Check if custom flag value changed after setting", [&]() {
            Hostname tmp = *fs.get<Hostname>("test_str");
            return tmp.name == "localhost" && tmp.port == 5001;
        }()},
    });

}