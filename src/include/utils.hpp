#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <iostream>
#include <string>
#include <sstream>
#include <map>

namespace pnt_cli::utils {
    // https://codereview.stackexchange.com/questions/48594/unique-type-id-no-rtti 
    class type_id_t {
            using sig = type_id_t();

            sig* id;
            type_id_t(sig* id) : id{id} {}

        public:
            template<typename T>
            friend type_id_t type_id();

            bool operator==(type_id_t o) const { return id == o.id; }
            bool operator!=(type_id_t o) const { return id != o.id; }
            // bool operator<(type_id_t o)  const { return id <  o.id; } // for std::map if needed
    };

    /**
     * @brief creates or returns a unique type_id_t for the given template parameter
     * 
     * @tparam T the type to get the type_id_t for
     * @return type_id_t a unique type id
     */
    template<typename T> type_id_t type_id() { return &type_id<T>; }

    template<typename T>
    concept Printable = requires (std::ostream& os, const T& t) {
        os << t;
    };
    template<typename T>
    concept InderectlyPrintable = requires (std::ostream& os, T t) {
        { os << *t };
    };

    template<Printable K, Printable V>
    inline std::string stringify_map(const std::map<K, V>& m) {
        std::stringstream ss;
        ss << "{";
        for (bool first = true; const auto& [k, v] : m) {
            if (!first) ss << ", ";
            ss << k << ": " << v;
            first = false;
        }
        ss << "}" << '\n';
        return ss.str();
    }

    template<Printable K, InderectlyPrintable V>
    inline std::string stringify_indirect_map(const std::map<K, V>& m) {
        std::stringstream ss;
        ss << "{";
        for (bool first = true; const auto& [k, v] : m) {
            if (!first) ss << ", ";
            ss << k << ": " << *v;
            first = false;
        }
        ss << "}" << '\n';
        return ss.str();
    }
} // namespace pnt_cli::utils

#endif // UTILS_HPP_