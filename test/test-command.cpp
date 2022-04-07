#include <iostream>

#include <test.hpp>
#include <command.hpp>



int main(int argc, char** argv) {
    using namespace pnt_cli;
    using namespace std;

    TestSuite tsuite("command.hpp tests");

    auto rootCmd = makeCommand("test", "this is a test command to test things", 
        [] (const Command* cmd, std::vector<std::string> args) {
            for (const auto& arg : args) {
                cout << arg << '\n';
            }
            return 0;
        }
    );


    tsuite.createGroup("Basic command functionality")
        .t("Freshly initialized flag should have no flags", !rootCmd->hasFlags())
        .t("Should have no parent", !rootCmd->hasParent());

    rootCmd->addLocalFlag<bool>("verbose", "enable verbose output", false);
    tsuite.createGroup("Basic local flag functionality")
        .t("Should have flags after insertion", rootCmd->hasFlags())
        .t("Check if 'verbose' flag can be found", rootCmd->getFlag<bool>("verbose"))
        .t("Check if 'verbose' flag value is the default", !(*rootCmd->getFlag<bool>("verbose")))
        .t("Check if 'verbose' flag can be set", rootCmd->setFlag<bool>("verbose", "true"))
        .t("Check if 'verbose' flag value changed after setting", *rootCmd->getFlag<bool>("verbose"));

    auto subCmd = rootCmd->addSubcommand("sub", "this is a sub command to test things",
        [] (const Command* cmd, std::vector<std::string> args) {
            for (const auto& arg : args) {
                cout << arg << '\n';
            }
            return 0;
        }
    );
    tsuite.createGroup("Basic subcommand functionality")
        .t("Command created through addSubcommand should have parent", subCmd->hasParent())
        .t("Freshly constructed subcommand should have no owned commands", !subCmd->hasFlags())
        .t("Root command should now have subcommands", rootCmd->hasSubcommands())
        .t("Root local flag should be invisible from subcommand", !subCmd->getFlag<bool>("verbose"));


    rootCmd->addPersistentFlag<int>("port", "port to listen on", 8080, "p");
    tsuite.createGroup("Basic persistent flag functionality")
        .t("Persistent flag should be visible from subcommand", !!subCmd->getFlag<int>("port"))
        .t("Persistent flag should be searchable via shorthand", !!subCmd->getFlag<int>("p"));

    return tsuite.run();
}