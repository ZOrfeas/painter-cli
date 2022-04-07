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
        test("Should have no flags", !rootCmd->hasFlags()),
        test("Should be root", rootCmd->isRoot())
    );

    rootCmd->addLocalFlag<bool>("verbose", "enable verbose output", false);
    tsuite("Basic local flag functionality",
        test("Should have flags", rootCmd->hasFlags()),
        test("Should have the verbose bool flag", rootCmd->getFlag<bool>("verbose")),
        test("Flag should be false", !(*rootCmd->getFlag<bool>("verbose"))),
        test("Should be able to set flag", rootCmd->setFlag<bool>("verbose", "true")),
        test("Flag should be true", *rootCmd->getFlag<bool>("verbose"))
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
        test("Should not be root", !subCmd->isRoot()),
        test("Should not have flags", !subCmd->hasFlags()),
        test("Should have subcommands", rootCmd->hasSubcommands()),
        test("Local root flag should not be visible from subcommand", !subCmd->getFlag<bool>("verbose"))
    );


    rootCmd->addPersistentFlag<int>("port", "port to listen on", 8080, "p");
    tsuite("Basic persistent flag functionality",
        test("Persistent flag should be visible from subcommand", subCmd->getFlag<int>("port")),
        test("Persistent flag should be searchable via shorthand", subCmd->getFlag<int>("p"))
    );
}