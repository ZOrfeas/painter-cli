#include <iostream>

#include <test.hpp>
#include <command.hpp>



int main(int argc, char** argv) {
    using namespace pnt_cli;
    using namespace std;

    auto rootCmd = makeCommand("test", "this is a test command to test things", 
        [] (const Command* cmd, std::vector<std::string> args) {
            for (const auto& arg : args) {
                cout << arg << '\n';
            }
            return 0;
        }
    );

    tsuite("Basic command functionality",{
        {"Freshly initialized flag should have no flags", !rootCmd->hasFlags()},
        {"Should have no parent", !rootCmd->hasParent()},
    });

    rootCmd->addLocalFlag<bool>("verbose", "enable verbose output", false);
    tsuite("Basic local flag functionality",{
        {"Should have flags after insertion", rootCmd->hasFlags()},
        {"Check if 'verbose' flag can be found", !!rootCmd->getFlag<bool>("verbose")},
        {"Check if 'verbose' flag value is the default", !(*rootCmd->getFlag<bool>("verbose"))},
        {"Check if 'verbose' flag can be set", rootCmd->setFlag<bool>("verbose", "true")},
        {"Check if 'verbose' flag value changed after setting", *rootCmd->getFlag<bool>("verbose")},
    });

    auto subCmd = rootCmd->addSubcommand("sub", "this is a sub command to test things",
        [] (const Command* cmd, std::vector<std::string> args) {
            for (const auto& arg : args) {
                cout << arg << '\n';
            }
            return 0;
        }
    );
    tsuite("Basic subcommand functionality",{
        {"Command created through addSubcommand should have parent", subCmd->hasParent()},
        {"Freshly constructed subcommand should have no owned commands", !subCmd->hasFlags()},
        {"Root command should now have subcommands", rootCmd->hasSubcommands()},
        {"Root local flag should be invisible from subcommand", !subCmd->getFlag<bool>("verbose")},
    });


    rootCmd->addPersistentFlag<int>("port", "port to listen on", 8080, "p");
    tsuite("Basic persistent flag functionality",{
        {"Persistent flag should be visible from subcommand", !!subCmd->getFlag<int>("port")},
        {"Persistent flag should be searchable via shorthand", !!subCmd->getFlag<int>("p")},
    });
}