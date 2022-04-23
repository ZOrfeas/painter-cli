#ifndef FSM_HPP_
#define FSM_HPP_

#include <iostream>
#include <array>
#include <utility>
#include <stdexcept>

namespace painter::utils {
    using std::array;
    using std::pair;
    using std::ostream;
    using std::invalid_argument;
    
    template<
        size_t StateCnt, size_t EventCnt
    > class FSM {
        private:
            class State {
                public:
                    size_t id;
                    constexpr State(size_t id) : id(id) {}
                    constexpr State() : id(StateCnt) {}
                    constexpr bool is_valid() const { return id < StateCnt; }
                    constexpr bool is_null() const { return id == StateCnt; }
                    constexpr operator size_t() const { return id; }
            };
            class Event {
                public:
                    size_t id;
                    constexpr Event(size_t id) : id(id) {}
                    constexpr Event() : id(EventCnt) {}
                    constexpr bool is_valid() const { return id < EventCnt; }
                    constexpr bool is_null() const { return id == EventCnt; }
                    constexpr operator size_t() const { return id; }
            };
            class Transition {
                public:
                    State from;
                    State to;
                    constexpr Transition(State from, State to) : from(from), to(to) {}
                    constexpr Transition() : from(), to() {}
                    constexpr bool is_null() const { return from.is_null() && to.is_null(); }
                    constexpr bool is_valid() const { return from.is_valid() && to.is_valid(); }
            };
        public:
            constexpr static size_t state_cnt = StateCnt;
            constexpr static size_t event_cnt = EventCnt;
        private:
            array<array<Transition, EventCnt>, StateCnt> state_transitions_;
        public:
            constexpr
            FSM(): state_transitions_(array<array<Transition, EventCnt>, StateCnt>()) {}

            /** @brief Accepts an array that for each state specifies the target given an event. */
            constexpr
            FSM(array<pair<State, array<pair<Event, State>, EventCnt>>, StateCnt> state_transitions): FSM() {
                for (auto const& [from, pairs] : state_transitions) {
                    if (from.is_null()) { break; }
                    if (!from.is_valid()) { throw invalid_argument("invalid from state"); }
                    for (auto const& [event, to] : pairs) {
                        if (event.is_null() && to.is_null()) { break; }
                        if (!event.is_valid()) { throw invalid_argument("invalid event"); }
                        if (!to.is_valid()) { throw invalid_argument("invalid target state"); }
                        if (state_transitions_[from][event].is_valid()) {
                            throw invalid_argument(std::to_string(from) 
                                    + " already has a transition for event "
                                    + std::to_string(event));
                        }
                        state_transitions_[from][event] = Transition(from, to);
                    }
                }
            }
            constexpr
            State advance(State from, Event event) const {
                if (!from.is_valid()) { throw invalid_argument("invalid from state"); }
                if (!event.is_valid()) { throw invalid_argument("invalid event"); }
                Transition& transition = state_transitions_[from][event];
                if (!transition.is_valid()) { 
                    throw invalid_argument(std::to_string(from) 
                            + " does not have a transition for event "
                            + std::to_string(event));
                }
                return transition.to;
            }
            friend
            ostream& operator<<(ostream& os, FSM const& fsm) {
                for (auto const& state : fsm.state_transitions_) {
                    auto state_id = &state - &fsm.state_transitions_[0];
                    os << "State " << state_id << ":\n";
                    for (auto const& transition : state) {
                        auto event_id = &transition - &state[0];
                        os << "\tEvent " << event_id << ": " << transition.from << " -> " << transition.to << '\n';
                    }
                }
                return os;
            }

            // different initialization, for each event an array of transitions
            // constexpr
            // FSM(array<pair<Event, array<Transition, StateCnt>>, EventCnt> const& event_transitions): FSM() {
            //     for (auto const& [event, transitions] : event_transitions) {
            //         if (!event.is_valid()) {
            //             throw invalid_argument("invalid event");
            //         }
            //         for (auto const& t : transitions) {
            //             if (t.is_null()) { break; }
            //             if (!t.is_valid()) { throw invalid_argument("invalid transition"); }
            //             if (state_transitions_[t.from][event].is_valid()) {
            //                 throw invalid_argument(std::to_string(t.from) + " already has a transition for event " + std::to_string(event));
            //             }
            //             state_transitions_[t.from][event] = t;
            //         }
            //     }
            // }

    };
    // namespace state {
    //     enum states : size_t {
    //         Start, End
    //     };
    // }
    // namespace event {
    //     enum events : size_t {
    //         Stay, Change
    //     };
    // }

}


#endif // FSM_HPP_