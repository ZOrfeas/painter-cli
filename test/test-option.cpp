
#include <iostream>
#include <concepts>
#include <option.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace painter::cli;

template<OptionType T>
class Defaults {
    public:
        static T get_value();
        static T get_another_value();
        static std::string get_name();
        static std::string get_description();
        static std::string get_shorthand();

        static std::string stringify_val(T const&);
};

static_assert(OptionType<int>);
static_assert(OptionType<float>);
static_assert(OptionType<std::string>);

template<> int Defaults<int>::get_value() { return 42; }
template<> float Defaults<float>::get_value() { return 3.14; }
template<> std::string Defaults<std::string>::get_value() { return "hello"; }

template<> int Defaults<int>::get_another_value() { return 24; }
template<> float Defaults<float>::get_another_value() { return 2.718; }
template<> std::string Defaults<std::string>::get_another_value() { return "world"; }

template<> std::string Defaults<int>::get_name() { return "intopt"; }
template<> std::string Defaults<float>::get_name() { return "floatopt"; }
template<> std::string Defaults<std::string>::get_name() { return "stropt"; }

template<> std::string Defaults<int>::get_description() { return "int option"; }
template<> std::string Defaults<float>::get_description() { return "float option"; }
template<> std::string Defaults<std::string>::get_description() { return "string option"; }

template<> std::string Defaults<int>::get_shorthand() { return "i"; }
template<> std::string Defaults<float>::get_shorthand() { return "f"; }
template<> std::string Defaults<std::string>::get_shorthand() { return "s"; }

class NonSerializableType {};

template<OptionType T>
class OptionTest : public ::testing::Test {
    protected:
        OptionTest() {
            option = make_shared<OptionImpl<T>>(
                Defaults<T>::get_name(),
                Defaults<T>::get_description(),
                Defaults<T>::get_value(),
                Defaults<T>::get_shorthand()
            );
        }
        shared_ptr<Option> option;
};

TYPED_TEST_SUITE_P(OptionTest);
TYPED_TEST_P(OptionTest, InitializesProperly) {
    EXPECT_NE(this->option, nullptr);
    OptionImpl<TypeParam>* opt = this->option->template as<TypeParam>();
    EXPECT_NE(opt, nullptr);
    EXPECT_EQ(opt->get(), Defaults<TypeParam>::get_value());
}
TYPED_TEST_P(OptionTest, TypeTagWorks) {
    if constexpr (std::is_same_v<TypeParam, int>) {
        EXPECT_TRUE(this->option->template type_tag_matches<int>());
        EXPECT_TRUE(this->option->template as<int>());
    } else {
        EXPECT_FALSE(this->option->template type_tag_matches<int>());
        EXPECT_DEATH(this->option->template as<int>(), "Type tag mismatch"); // note message must much output
    }
}

TYPED_TEST_P(OptionTest, BasicFunctionalityWorks) {
    OptionImpl<TypeParam>* opt = this->option->template as<TypeParam>();
    EXPECT_NE(opt, nullptr);
    EXPECT_EQ(opt->get(), Defaults<TypeParam>::get_value());
    opt->set(serialize(Defaults<TypeParam>::get_another_value()));
    EXPECT_EQ(opt->get(), Defaults<TypeParam>::get_another_value());
}

REGISTER_TYPED_TEST_SUITE_P(OptionTest,
    InitializesProperly,
    TypeTagWorks,
    BasicFunctionalityWorks
);

struct Hostname {
    std::string ip;
    int port;
    friend std::ostream& operator<<(std::ostream& os, Hostname const& h) {
        return os << h.ip << ":" << h.port;
    }
    friend std::istream& operator>>(std::istream& is, Hostname& h) {
        std::string fullstr;
        is >> fullstr;
        auto splitr = fullstr.find(':');
        if (splitr == std::string::npos) {
            throw std::runtime_error("invalid hostname");
        }
        h.ip = fullstr.substr(0, splitr);
        h.port = std::stoi(fullstr.substr(splitr + 1));
        return is;
    }
    // needed for uniform test
    friend bool operator==(Hostname const& lhs, Hostname const& rhs) {
        return lhs.ip == rhs.ip && lhs.port == rhs.port;
    }
};
static_assert(OptionType<Hostname>);

template<> Hostname Defaults<Hostname>::get_value() { return {.ip="localhost", .port=8080}; }
template<> Hostname Defaults<Hostname>::get_another_value() { return {.ip="192.168.1.2", .port=9090}; }
template<> std::string Defaults<Hostname>::get_name() { return "hostname"; }
template<> std::string Defaults<Hostname>::get_description() { return "hostname option"; }
template<> std::string Defaults<Hostname>::get_shorthand() { return "h"; }


using BuiltInTypes = ::testing::Types<int, float, std::string>;
INSTANTIATE_TYPED_TEST_SUITE_P(BuiltInTest, OptionTest, BuiltInTypes);

using UserAddedTypes = ::testing::Types<Hostname>;
INSTANTIATE_TYPED_TEST_SUITE_P(UserAddedTest, OptionTest, UserAddedTypes);