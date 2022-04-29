#include <functional>
#include <fsm.hpp>
using namespace std;
using namespace painter::utils;

#include <gtest/gtest.h>

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
class FSMTest : public ::testing::Test {
    protected:
    using FSM = FSM<2, 2>;
        static constexpr FSM fsm = FSM({{
            {state::Start, {{
                {event::Stay, state::Start},
                {event::Change, state::End}
            }}},
            {state::End, {{
                {event::Stay, state::End},
                {event::Change, state::Start}
            }}}
        }});
        std::function<FSM::State(FSM::Event const&)> statefulFSM = 
            [current = FSM::State(state::Start)](FSM::Event const& event) mutable -> FSM::State {
                if (event.is_null()) { return current; } // returns current state
                current = FSMTest::fsm.advance(current, event);
                return current;
            };
            
};
TEST_F(FSMTest, AdvanceStateless) {
    EXPECT_EQ(state::Start, fsm.advance(state::Start, event::Stay));
    EXPECT_EQ(state::End, fsm.advance(state::Start, event::Change));
    EXPECT_EQ(state::End, fsm.advance(state::End, event::Stay));
    EXPECT_EQ(state::Start, fsm.advance(state::End, event::Change));
}
TEST_F(FSMTest, AdvanceStateful) {
    EXPECT_EQ(state::Start, statefulFSM(FSM::Event()));
    EXPECT_EQ(state::Start, statefulFSM(event::Stay));
    EXPECT_EQ(state::End, statefulFSM(event::Change));
    EXPECT_EQ(state::End, statefulFSM(event::Stay));
    EXPECT_EQ(state::Start, statefulFSM(event::Change));
}

TEST_F(FSMTest, AdvanceInvalids) {
    EXPECT_THROW(fsm.advance((size_t)state::End + 1, event::Stay), std::invalid_argument);
    EXPECT_THROW(fsm.advance(state::Start, (size_t)event::Change + 1), std::invalid_argument);
    
}

// int main() {
//     // constexpr auto fsm = FSM({{
//     //     {state::Start, 
//     //         {{
//     //             {event::Stay, state::Start},
//     //         }}
//     //     },
//     //     {state::End, 
//     //         {{
//     //             {event::Stay, state::End},
//     //         }}
//     //     }
//     // }});
//     // cout << fsm.advance(state::Start, event::Stay) << endl;
//     // cout << fsm.advance(state::Start, event::Change) << endl;
//     cout << fsm;
// }