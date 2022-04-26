#include <cstdio>
using namespace std;

// constexpr
// void init_cli() {
//     constexpr auto root_subcommands = array<string_view, 2>{"sub1"sv, "sub2"sv};
//     constexpr auto root_persistent = array<string_view, 2>{"-h"sv, "-g"sv};
//     constexpr auto root_local = array<string_view, 2>{"-f"sv, "--stop"sv};

//     constexpr auto sub2_subcommands = array<string_view, 1>{"sub3"sv};
//     constexpr auto sub2_persistent = array<string_view, 1>{"-t"sv};
//     constexpr auto sub2_local = array<string_view, 1>{"--report"sv};
// }

int main() {
    // test creating an FSM programmatically based on some other input
    // *root:
    //    +persistent:
    //        (fl)-h/--help, (opt)-g <int>
    //    -local:
    //        (opt)-f <string>, (fl)--stop
    //    *sub1:
    //    *sub2:
    //        +persistent:
    //            (opt)-t <int>
    //        -local:
    //            (fl)--report
    //        *sub3:

    constexpr auto tmp = [] {
        return "tmp";
    };
    constexpr auto tmp1 = tmp();
    printf("%s\n", tmp1);
    // cout << tmp1 << endl;

    return 0;
}