
#include <iostream>
#include <span>

int main(int argc, char **argv) {
    // using namespace painter::cli;
    std::span<char*> args(argv, argc);
    for (auto const& arg : args) {
        std::cout << arg << '\n';        
    }
}