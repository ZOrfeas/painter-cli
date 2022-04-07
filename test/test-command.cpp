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

    tsuite("Basic command functionality",
        test("Freshly initialized flag should have no flags", !rootCmd->hasFlags()),
        test("Should have no parent", !rootCmd->hasParent())
    );

    rootCmd->addLocalFlag<bool>("verbose", "enable verbose output", false);
    tsuite("Basic local flag functionality",
        test("Should have flags after insertion", rootCmd->hasFlags()),
        test("Check if 'verbose' flag can be found", rootCmd->getFlag<bool>("verbose")),
        test("Check if 'verbose' flag value is the default", !(*rootCmd->getFlag<bool>("verbose"))),
        test("Check if 'verbose' flag can be set", rootCmd->setFlag<bool>("verbose", "true")),
        test("Check if 'verbose' flag value changed after setting", *rootCmd->getFlag<bool>("verbose"))
    );

    auto subCmd = rootCmd->addSubcommand("sub", "this is a sub command to test things",
        [] (const Command* cmd, std::vector<std::string> args) {
            for (const auto& arg : args) {
                cout << arg << '\n';
            }
            return 0;
        }
    );
    tsuite("Basic subcommand functionality",
        test("Command created through addSubcommand should have parent", subCmd->hasParent()),
        test("Freshly constructed subcommand should have no owned commands", !subCmd->hasFlags()),
        test("Root command should now have subcommands", rootCmd->hasSubcommands()),
        test("Root local flag should be invisible from subcommand", !subCmd->getFlag<bool>("verbose"))
    );


    rootCmd->addPersistentFlag<int>("port", "port to listen on", 8080, "p");
    tsuite("Basic persistent flag functionality",
        test("Persistent flag should be visible from subcommand", subCmd->getFlag<int>("port")),
        test("Persistent flag should be searchable via shorthand", subCmd->getFlag<int>("p"))
    );
}