#include <iostream>
#include <array>
#include <vector>
#include <functional>
#include <memory>
#include <string_view>

#include <mapbox_eternal.hpp>

#include <utils.hpp>
#include <fsm.hpp>

using namespace std;
using namespace painter::utils;

template<
    size_t LocalFlagCnt, size_t LocalOptCnt,
    size_t PersFlagCnt, size_t PersOptCnt,
    size_t SubCmdCnt
> class Command {
public:
    using type = Command<LocalFlagCnt, LocalOptCnt, PersFlagCnt, PersOptCnt, SubCmdCnt>;
private:
    string_view name_;
    string_view description_;
    array<string_view, LocalFlagCnt> local_flags_;
    array<string_view, LocalOptCnt> local_opts_;
    array<string_view, PersFlagCnt> persistent_flags_;
    array<string_view, PersOptCnt> persistent_opts_;
    array<cnst_shared_ptr<type>, SubCmdCnt> subcommands_;
public:
    constexpr Command(
        string_view name = "root",
        string_view description = "",
        array<string_view, LocalFlagCnt> const& local_flags = {},
        array<string_view, LocalOptCnt> const& local_opts = {},
        array<string_view, PersFlagCnt> const& persistent_flags = {},
        array<string_view, PersOptCnt> const& persistent_opts = {},
        array<cnst_shared_ptr<type>, SubCmdCnt> const& subcommands = {}
    ) : name_(name), description_(description),
        local_flags_(local_flags), local_opts_(local_opts),
        persistent_flags_(persistent_flags),
        persistent_opts_(persistent_opts),
        subcommands_(subcommands) {}
    // constexpr ~Command() {
    //     for (auto const* subcmd : subcommands_) {
    //         delete subcmd;
    //     }
    // }
    
};
// TODO: Try out the following:
constexpr
auto init_cli() {
    constexpr size_t maxLocalFlagCnt = 3;
    constexpr size_t maxLocalOptCnt = 3;
    constexpr size_t maxPersistentFlagCnt = 3;
    constexpr size_t maxPersistentOptCnt = 3;
    constexpr size_t maxSubCmdCnt = 1;
    using Command = Command<
        maxLocalFlagCnt, maxLocalOptCnt,
        maxPersistentFlagCnt, maxPersistentOptCnt,
        maxSubCmdCnt
    >;
    // auto root = cnst_shared_ptr(new Command(
    //     "root", "the root command",
    //     {"-["}, {"-]"}, {"-p", "-f"}, {"-o"},
    //     {
    //         new Command(
    //             "sub", "the sub command",
    //             {"-f"}, {}, {}, {}, {}
    //         )
    //     }
    // ));
    auto tmp = cnst_vector<int>(1);
    tmp.push_back(1);
    tmp.push_back(2);
    tmp.push_back(3);
    tmp.push_back(4);
    tmp.push_back(5);
    return tmp.capacity();
}

// template<
//     class derived
// > class Cli {
// private:
//     static constexpr auto str_to_event_ = []{
//         return mapbox::eternal::map<string_view, size_t>({
//             {"a", 1},
//             // {"b", 2},
//             // ...
//         });
//     }();
//     static constexpr auto str_to_state_ = []{
//         return mapbox::eternal::map<string_view, size_t>({
//             {"a", 1},
//             // {"b", 2},
//             // ...
//         });
//     }();
//     static constexpr painter::utils::FSM<derived::state_cnt, derived::event_cnt>; 
//     // declare the constructor as private, 
//     // and UserBase as a friend class, to prevent
//     // the user from creating an instance of Cli
//     // and to only allow deriving from the specialization of ones' self
//     // Cli() {};
//     // friend UserBase;
// };

int main() {
    // constexpr auto tmp = mapbox::eternal::map<string_view, size_t>({
    //     {"a", 1},
    //     {"b", 2},
    //     {"c", 3}
    // });;
    // auto it = tmp.find("a");
    // if (it != tmp.end()) {
    //     cout << it->second << endl;
    // }

    constexpr auto tmp = (init_cli());
    cout << tmp << endl;

    return 0;
}