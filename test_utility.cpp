#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>
#include "gtest/gtest.h"

#include "md5.h"
#include "crc32.h"

#include "File.hpp"
#include "filter_files.hpp"

namespace
{

int in_argc{0};
char** in_argv{nullptr};

}

TEST(TEST_HASH_LIB, test_md5)
{
    MD5 hasher;
    std::string s{"The quick brown fox jumps over the lazy dog."};
    hasher.add(s.c_str(), s.size());
    auto res = hasher.getHash();
    ASSERT_TRUE(res == std::string_view("e4d909c290d0fb1ca068ffaddf22cbd0"));

    s = "The quick brown fox jumps over the lazy dog";
    hasher.reset();
    hasher.add(s.c_str(), s.size());
    res = hasher.getHash();
    ASSERT_TRUE(res == std::string_view("9e107d9d372bb6826bd81d3542a419d6"));

    s.clear();
    hasher.reset();
    hasher.add(s.c_str(), s.size());
    res = hasher.getHash();
    ASSERT_TRUE(res == std::string_view("d41d8cd98f00b204e9800998ecf8427e"));
}

TEST(TEST_HASH_LIB, test_crc32)
{
    CRC32 hasher;
    std::string s{"The quick brown fox jumps over the lazy dog."};
    hasher.add(s.c_str(), s.size());
    auto res = hasher.getHash();
    ASSERT_TRUE(res == std::string_view("519025e9"));

    s = "The quick brown fox jumps over the lazy dog";
    hasher.reset();
    hasher.add(s.c_str(), s.size());
    res = hasher.getHash();
    ASSERT_TRUE(res == std::string_view("414fa339"));

    s.clear();
    hasher.reset();
    hasher.add(s.c_str(), s.size());
    res = hasher.getHash();
    ASSERT_TRUE(res == std::string_view("00000000"));
}

TEST(TEST_HASH_CLASS, test_class_md5)
{
    namespace fs = std::filesystem;
    try
    {
    {
        File file{fs::path{fs::absolute("../tests/tests_for_File/fox-with-dot.txt")}, HashAlgorithm::MD5};
        File::block_size(file.size());
        const auto hash{file.update_hash()};
        File::file_hash_t ref{File::hash_value_t{0xe4, 0xd9, 0x09, 0xc2, 0x90, 0xd0, 0xfb, 0x1c,
                                                 0xa0, 0x68, 0xff, 0xad, 0xdf, 0x22, 0xcb, 0xd0}};
        ASSERT_TRUE(hash == ref);
    }
    {
        File file{fs::path{fs::absolute("../tests/tests_for_File/fox-without-dot.txt")}, HashAlgorithm::MD5};
        File::block_size(file.size());
        const auto hash{file.update_hash()};
        File::file_hash_t ref{File::hash_value_t{0x9e, 0x10, 0x7d, 0x9d, 0x37, 0x2b, 0xb6, 0x82,
                                                 0x6b, 0xd8, 0x1d, 0x35, 0x42, 0xa4, 0x19, 0xd6}};
        ASSERT_TRUE(hash == ref);
    }
    {
        File file{fs::path{fs::absolute("../tests/tests_for_File/empty.txt")}, HashAlgorithm::MD5};
        File::block_size(file.size());
        const auto hash{file.update_hash()};
        File::file_hash_t ref{File::hash_value_t{0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
                                                 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e}};
        ASSERT_TRUE(hash == ref);
    }
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

TEST(TEST_BAYAN, test_main_actions)
{
    // The iterartion order for std::filesystem::recursive_directory_iterator is not specified,
    // thus we sort each block of file to compare it against reference result.
    auto cmp = [](const std::string& l, const std::string& r)
    {
        return std::lexicographical_compare(std::begin(l), std::end(l),
                                            std::begin(r), std::end(r));
    };
    auto sort = [&cmp](ordered_files_t& ordered_files)
    {
        for(auto& fset:ordered_files)
            std::sort(std::begin(fset), std::end(fset), cmp);
    };
    auto to_filename = [](ordered_files_t& ordered_files)
    {
        for(auto& fset:ordered_files)
        {
            for(auto& file:fset)
                file = std::filesystem::path{file}.filename().string();
        }
    };
    auto print = [](const ordered_files_t& ordered_files)
    {
        for(const auto& fset:ordered_files)
        {
            for(const auto& file:fset)
                std::cout << file << '\n';
            std::cout << '\n';
        }
    };

    auto compare_file_sets = [](const ordered_files_t& l, const ordered_files_t& r)
    {
        auto find = [](const ordered_files_t::value_type& fset, const ordered_files_t& ordered_files)
        {
            for(const auto& rfset:ordered_files)
            {
                if(fset == rfset)
                    return true;;
            }
            return false;
        };
        if(l.size() != r.size())
            return false;
        for(const auto& fset:l)
        {
            if(!find(fset, r))
                return false;
        }
        return true;
    };

    {
        constexpr int argc{11};
        const char* argv[argc]{in_argv[0],
                    "-i",
                    "../tests",
                    "-H",
                    "crc32",
                    "-d",
                    "5",
                    "-b",
                    "20",
                    "-r",
                    "(.*pic18f.*)"
        };
        auto ordered_files{bayan(argc, argv)};
        to_filename(ordered_files);
        sort(ordered_files);
        print(ordered_files);
        ordered_files_t ref{{"pic18f-1.txt", "pic18f-3.txt"}, {"pic18f-2.txt"}};
        ASSERT_TRUE(compare_file_sets(ordered_files, ref));
    }

    {
        constexpr int argc{11};
        const char* argv[argc]{in_argv[0],
                    "-i",
                    "../tests",
                    "-H",
                    "md5",
                    "-d",
                    "5",
                    "-b",
                    "20",
                    "-r",
                    "(Data.*|.*be supers.*|.*only-2.txt|.*gh Perf.*)"
        };
        auto ordered_files{bayan(argc, argv)};
        to_filename(ordered_files);
        sort(ordered_files);
        print(ordered_files);
        ordered_files_t ref{{"Data Sheet-1.txt", "and may be superseded-3.txt"},
                            {"High Performance-2.txt", "through suggestion only-2.txt"}
                           };
        ASSERT_TRUE(compare_file_sets(ordered_files, ref));
    }
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    in_argc = argc;
    in_argv = argv;
    return RUN_ALL_TESTS();
}
