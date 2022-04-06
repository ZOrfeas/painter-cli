#include <iostream>

#include <log.hpp>
#include <command.hpp>

int main(int argc, char** argv) {
    using namespace pnt_cli;
    using namespace std;
    log_m("Starting command.hpp tests");

    auto rootCmd = makeCommand("test", "this is a test command to test things", 
        [] (const Command* cmd, std::vector<std::string> args) {
            for (const auto& arg : args) {
                cout << arg << '\n';
            }
            return 0;
        }
    );
    test_if("Should have no flags", !rootCmd->hasFlags());
    test_if("Should be root", rootCmd->isRoot());

    rootCmd->addSubcommand("sub", "this is a sub command to test things",
        [] (const Command* cmd, std::vector<std::string> args) {
            for (const auto& arg : args) {
                cout << arg << '\n';
            }
            return 0;
        }
    );

    rootCmd->addLocalFlag<bool>("verbose", "enable verbose output", false);
    test_if("Should have flags", rootCmd->hasFlags());
    test_if("Should have the verbose bool flag", rootCmd->getFlag<bool>("verbose"));
    test_if("Flag should be false", !(*rootCmd->getFlag<bool>("verbose")));
    test_if("Should be able to set flag", rootCmd->setFlag<bool>("verbose", "true"));
    test_if("Flag should be true", *rootCmd->getFlag<bool>("verbose"));


    log_m("All tests pass");
    // return rootCmd.execute(argc, argv);

}