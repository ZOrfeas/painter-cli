#ifndef FLOPT_HPP_
#define FLOPT_HPP_

#include <string_view>

#include <utils.hpp>

namespace painter::cli {
    using namespace painter::utils;

    class FloptBase {
    protected:
    public:
        std::string_view const name;
        std::string_view const description;
        std::string_view const shorthand;
        bool const is_flag;
        bool const is_option;
        
        friend class FloptSet;
        constexpr FloptBase(
            std::string_view, std::string_view,
            std::string_view, bool
        );
        constexpr virtual ~FloptBase() = default;

        /**
         * @brief Deserializes the provided string into the memory 
         * location provided based on the type of the instance.
         */
        virtual void set(void*, std::string_view) const = 0;
    };


    template<Deserializable T>
    class Flopt: public FloptBase {
    private:
    public:
        T const value;
        constexpr Flopt(
            std::string_view, std::string_view,
            T const&, std::string_view = ""
        );
        constexpr ~Flopt() = default;

        void set(void*, std::string_view) const override;
    };

    /////////////////////////////////////////////////////
    // FloptBase class member function implementations //
    /////////////////////////////////////////////////////

    constexpr inline FloptBase::FloptBase(
        std::string_view name,
        std::string_view description,
        std::string_view shorthand,
        bool is_flag
    ):  name(name), description(description),
        shorthand(shorthand), is_flag(is_flag), is_option(!is_flag) {}

    /////////////////////////////////////////////////
    // Flopt class member function implementations //
    /////////////////////////////////////////////////

    template<Deserializable T>
    constexpr inline Flopt<T>::Flopt(
        std::string_view name,
        std::string_view description,
        T const& default_value,
        std::string_view shorthand
    ): FloptBase(name, description, shorthand, std::is_same_v<T, bool>),
        value(default_value) {}

    template<Deserializable T>
    void Flopt<T>::set(void* ptr, std::string_view value) const {
        *static_cast<T*>(ptr) = deserialize<T>(value);
    }
}


#endif // FLOPT_HPP_