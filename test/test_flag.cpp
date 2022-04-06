#include <iostream>
#include <string>
#include <sstream>

#include <log.hpp>
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

// template<>
// std::string pnt_cli::toString<Hostname>(const Hostname& val) {
//     return val.name + ":" + std::to_string(val.port);
// }


int main() {
    using namespace pnt_cli;
    using namespace std;

    static_assert(FlagType<int>);
    static_assert(FlagType<float>);
    static_assert(FlagType<std::string>);
    static_assert(FlagType<bool>);

    FlagSet fs;
    test_pred("Should be empty", fs.empty);
    
    fs.addFlag<int>("t_int", "int flag", 1, "i");
    fs.addFlag<float>( "t_float", "float flag", 1.0f, "f");

    test_pred("Should not be empty", !fs.empty);
    test_if("Should have 2 flags", fs.size() == 2);

    test_if("Flag 't_int' should exist", fs.find<int>("t_int"));
    test_if("Flag 't_int' has the default value", fs.get<int>("t_int") == 1);
    test_if("Flag 't_int' should be set", fs.set<int>("t_int", "2"));
    test_if("Flag 't_int' should have the new value", fs.get<int>("t_int") == 2);

    fs.addFlag<Hostname>("test_str", "test custom flag", fromString<Hostname>("localhost:5000"), "s");
    test_if("Should have 3 flags", fs.size() == 3);
    test_if("Flag 'test_str' should exist", fs.get<Hostname>("test_str"));
    test_pred("Flag 'test_str' has the default value", [&]() {
        Hostname tmp = *fs.get<Hostname>("test_str");
        return tmp.name == "localhost" && tmp.port == 5000;
    });
    test_if("Flag 'test_str' should be set", fs.set<Hostname>("test_str", "localhost:5001"));
    test_pred("Flag 'test_str' should have the new value", [&]() {
        Hostname tmp = *fs.get<Hostname>("test_str");
        return tmp.name == "localhost" && tmp.port == 5001;
    });

    log_m("All tests pass");
}