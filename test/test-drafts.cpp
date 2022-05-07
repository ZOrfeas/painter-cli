#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <string_view>

#include <mapbox_eternal.hpp>

#include <utils.hpp>
#include <fsm.hpp>

using namespace std;


template<typename T>
struct TypeContainer {
    using type = T;
};

// TODO: Try out the following:
constexpr
auto init_cli() {
    return 0;
}

template<typename T>
constexpr
auto register_type(std::string_view name) {
    return mapbox::eternal::map({
        {name, TypeContainer<T>{}},
        {name, TypeContainer<bool>{}}
    });
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

    constexpr auto tmp = init_cli();
    cout << tmp << endl;
    return 0;
}