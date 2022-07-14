// homework #7: bayan
// find identical files
// print: paths to identical files, separate groups of files by emty line

#include <iostream>
#include "filter_files.hpp"

int main(int argc, const char *argv[])
{
    try
    {
        const auto& ordered_files{bayan(argc, argv)};
        for(const auto& fset:ordered_files)
        {
            for(const auto& file:fset)
                std::cout << file << '\n';
            std::cout << '\n';
        }
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
