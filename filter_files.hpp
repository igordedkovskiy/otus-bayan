#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <string>

#include "hash.hpp"
#include "File.hpp"

using size_type = std::size_t;
using folders_t = std::vector<File::path_t>;
using files_t = std::vector<File>;
using ordered_files_t = std::vector<std::vector<std::string>>;
using mask_t = std::string;

files_t make_set_of_files(const folders_t &to_discover, const folders_t &to_ommit, size_type file_size,
                          const mask_t &masks, size_type depth,
                          const std::string& hash_algo, size_type block_size);

ordered_files_t arrange_files(files_t& files);

ordered_files_t bayan(int argc, const char *argv[]);
