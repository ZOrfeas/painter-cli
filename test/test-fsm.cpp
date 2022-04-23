#include <fsm.hpp>
using namespace std;
using namespace painter::utils;
// #include <gtest/gtest.h>

// class FSMTest : public ::testing::Test {

// }
namespace state {
    enum states : size_t {
        Start, End
    };
}
namespace event {
    enum events : size_t {
        Stay, Change
    };
}

int main() {
    using FSM = FSM<2, 2>;
    constexpr auto fsm = FSM({{
        {state::Start, 
            {{
                {event::Stay, state::Start},
            }}
        },
        {state::End, 
            {{
                {event::Stay, state::End},
            }}
        }
    }});
    // cout << fsm.advance(state::Start, event::Stay) << endl;
    // cout << fsm.advance(state::Start, event::Change) << endl;
    cout << fsm;

}