#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <string>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <concepts>

namespace painter::utils {

    template<typename T> 
    concept Deserializable = requires(std::istream& is, T& t) {
        { is >> t } -> std::same_as<std::istream&>;
    };
    template<Deserializable T>
    inline T deserialize(std::string const& s) {
        std::istringstream iss(s);
        T t;
        iss >> t;
        return t;
    }

    template<typename T>
    concept Serializable = requires(std::ostream& os, T const& t) {
        { os << t } -> std::same_as<std::ostream&>;
    };
    template<Serializable T>
    inline std::string serialize(T const& t) {
        std::ostringstream oss;
        oss << t;
        return oss.str();
    }

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
    template<typename T> 
    inline type_id_t type_id() { return &type_id<T>; }


}

#endif // UTILS_HPP_