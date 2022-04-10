#include <iostream>

// #include <test.hpp>
#include <command.hpp>
#include <gtest/gtest.h>

using namespace pnt_cli;
using namespace std;

auto someDefaultAction = [] (Command const& cmd, std::vector<std::string> args) {
    return 0;
};

class CommandTest : public ::testing::Test {
    protected:
        static const std::string localFlagName;
        static const std::string localFlagDescription;
        static const bool localFlagDefaultValue;

        static const std::string persistentFlagName;
        static const std::string persistentFlagDescription;
        static const bool persistentFlagDefaultValue;
        static const std::string persistentFlagShorthand;

        shared_ptr<Command> rootCmd;
        shared_ptr<Command> subCmd;

        void SetUp() override {
            rootCmd = makeCommand("some_command", "some description", someDefaultAction);
        }
        void addLocalFlagToRoot() {
            rootCmd->addLocalFlag<bool>(localFlagName, localFlagDescription, localFlagDefaultValue);
        }
        void addPersistentFlagToRoot() {
            rootCmd->addPersistentFlag<bool>(persistentFlagName, persistentFlagDescription, persistentFlagDefaultValue, persistentFlagShorthand);
        }
        void addPersistentFlagToSub() {
            subCmd->addPersistentFlag<bool>(persistentFlagName, persistentFlagDescription, persistentFlagDefaultValue, persistentFlagShorthand);
        }
        void addSubcommandToRoot() {
            subCmd = makeCommand("sub_command", "sub command description", someDefaultAction);
            rootCmd->addSubcommand(subCmd);
        }
};
const std::string CommandTest::localFlagName = "local_flag";
const std::string CommandTest::localFlagDescription = "local flag description";
const bool CommandTest::localFlagDefaultValue = false;
const std::string CommandTest::persistentFlagName = "global_flag";
const std::string CommandTest::persistentFlagDescription = "global flag description";
const bool CommandTest::persistentFlagDefaultValue = false;
const std::string CommandTest::persistentFlagShorthand = "g";

TEST_F(CommandTest, InitializesProperly) {
    EXPECT_FALSE(rootCmd->hasFlags());
    EXPECT_FALSE(rootCmd->hasParent());
}

TEST_F(CommandTest, LocalFlagsWork) {
    addLocalFlagToRoot();
    EXPECT_TRUE(rootCmd->hasFlags());
    EXPECT_TRUE(rootCmd->getFlag<bool>(localFlagName));
    EXPECT_EQ((*rootCmd->getFlag<bool>(localFlagName)), localFlagDefaultValue);
    EXPECT_TRUE(rootCmd->setFlag<bool>(localFlagName, toString<bool>(!localFlagDefaultValue)));
    EXPECT_EQ((*rootCmd->getFlag<bool>(localFlagName)), !localFlagDefaultValue);
    EXPECT_THROW(addLocalFlagToRoot(),std::runtime_error);
}

TEST_F(CommandTest, SubCommandWorks) {
    addSubcommandToRoot();
    EXPECT_TRUE(rootCmd->hasSubcommands());
    EXPECT_FALSE(subCmd->hasSubcommands());
    EXPECT_TRUE(subCmd->hasParent());
    EXPECT_FALSE(subCmd->hasFlags());
    addLocalFlagToRoot();
    EXPECT_FALSE(subCmd->getFlag<bool>(localFlagName));
}

TEST_F(CommandTest, PersistentFlagsWork) {
    addPersistentFlagToRoot();
    addSubcommandToRoot();
    EXPECT_TRUE(subCmd->getFlag<bool>(persistentFlagName));
    EXPECT_TRUE(subCmd->getFlag<bool>(persistentFlagShorthand));
    EXPECT_THROW(addPersistentFlagToSub(), std::runtime_error);
}