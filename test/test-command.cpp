#include <string_view>
#include <utility>

#include <utils.hpp>
#include <gtest/gtest.h>

#include <command.hpp>

using namespace painter::cli;

class Defaults {
public:
    std::string_view const name = "command", sub_name = "subcommand";
    std::string_view const description = "command description", sub_description = "subcommand description";
    ActionFn const action = []() { return 0; };

    constexpr Defaults() {}
};

class TestCommand : public ::testing::Test {
protected:
    static constexpr Defaults d{};
    static constexpr auto single_root() {
        return Command(
            d.name, d.description, d.action
        );
    }
    static constexpr auto root_add_sub(Command& root) {
        return root.add_subcommand(
            d.sub_name, d.sub_description, d.action
        );
    }
    static constexpr auto initializes_properly() {
        auto root = single_root();
        return root.name == d.name &&
            root.description == d.description &&
            root.action() == 0;
    }
    static constexpr auto add_command_correct() {
        auto root = single_root();
        auto success = root_add_sub(root); 
        return success && root.subcommands.size() == 1 &&
            root.subcommands[0]->name == d.sub_name &&
            root.subcommands[0]->description == d.sub_description;
    }
    static constexpr auto add_command_same_name() {
        auto root = single_root();
        auto success = root.add_subcommand(
            d.name, d.sub_description, d.action
        );
        return !success && root.subcommands.size() == 0;
    }
    static constexpr auto add_local_flopt() {
        auto root = single_root();
        auto success = root.add_local_flopt<int>(
            "int_option", "int option", 42, "i"
        );
        return success && root.local_flopts.size() == 1 &&
            root.flag_cnt() == 0 &&
            root.option_cnt() == 1 &&
            root.local_flopts[0]->name == "int_option" &&
            root.local_flopts[0]->description == "int option" &&
            root.local_flopts[0]->shorthand == "i" &&
            root.local_flopts[0]->is_flag == false &&
            root.local_flopts[0]->is_option == true &&
            static_cast<Flopt<int>*>(root.local_flopts[0].get())->value == 42;
    } 
    static constexpr auto add_persistent_flopt() {
        auto root = single_root();
        root_add_sub(root);
        auto sub_handle = *(root.subcommands.end() - 1);
        auto success = root.add_persistent_flopt<bool>(
            "bool_flag", "bool flag", false, "b"
        );
        auto flag_handle = (root.persistent_flopts.end() - 1)->get();

        return success &&
            sub_handle->flag_cnt() == 1 && 
            sub_handle->option_cnt() == 0 &&
            sub_handle->persistent_flopts.size() == 0 &&
            root.persistent_flopts.size() == 1 &&
            flag_handle->name == "bool_flag" &&
            flag_handle->description == "bool flag" &&
            flag_handle->shorthand == "b" &&
            static_cast<Flopt<bool>*>(flag_handle)->value == false;

    }
};
TEST_F(TestCommand, InitiliazesProperly) {
    CONSTEXPR_AND_EXPECT_TRUE(initializes_properly());
}
TEST_F(TestCommand, AddCommandWorks) {
    CONSTEXPR_AND_EXPECT_TRUE(add_command_correct());
    CONSTEXPR_AND_EXPECT_TRUE(add_command_same_name());
}
TEST_F(TestCommand, AddFloptWorks) {
    CONSTEXPR_AND_EXPECT_TRUE(add_local_flopt());
    CONSTEXPR_AND_EXPECT_TRUE(add_persistent_flopt());
}