#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <filesystem>
#include <regex>
#include <memory>
#include <boost/program_options.hpp>

#include "input.hpp"
#include "filter_files.hpp"
#include "File.hpp"

files_t make_set_of_files(const folders_t &to_discover, const folders_t &to_ommit, size_type file_size,
                          const mask_t &masks, size_type depth,
                          const std::string& hash_algo, size_type block_size)
{
    namespace fs = std::filesystem;
    using path_t = File::path_t;

    files_t set_of_files;
    std::hash<path_t> path_hash;
    std::unordered_set<decltype(path_hash(path_t{}))> paths_to_ommit;
    for(const auto& folder:to_ommit)
        paths_to_ommit.insert(path_hash(fs::absolute(folder)));
    File::block_size(block_size);
    const auto halgo{to_hash_algo_name(hash_algo)};
    const auto regexp_cmp{std::regex(masks)};
    for(const auto& folder:to_discover)
    {
        try
        {
            const auto path{fs::absolute(folder)};
            const auto phash{path_hash(path)};
            auto el{paths_to_ommit.find(phash)};
            if(el != paths_to_ommit.end())
                continue;
            paths_to_ommit.insert(phash);
            using recursive_iterator = fs::recursive_directory_iterator;
            for(auto it{recursive_iterator(path)}; it != recursive_iterator(); ++it)
            {
                if(it.depth() > static_cast<decltype(it.depth())>(depth))
                    continue;
                auto path{fs::absolute(it->path())};
                if(!fs::is_directory(path))
                {
//                    if(fs::file_size(path) >= file_size && regexp_cmp(masks, path.filename().string()))
                    if(fs::file_size(path) >= file_size && regex_match(path.filename().string(), regexp_cmp))
                        set_of_files.emplace_back(File{std::move(path), halgo});
                }
            }
        }
        catch(const fs::filesystem_error& e)
        {
            std::cerr << e.what() << std::endl;
            continue;
        }
    }
    return set_of_files;
}

ordered_files_t arrange_files(files_t& files)
{
    ordered_files_t ordered_files;
    std::vector<bool> processed(files.size(), false);
    std::size_t cntr{0};
    for(auto it{std::begin(files)}; it != std::end(files); ++it, ++cntr)
    {
        if(processed[cntr])
            continue;
        processed[cntr] = true;
        //ordered_files.emplace_back(std::vector<std::reference_wrapper<File>>{});
        //ordered_files.back().emplace_back(std::ref(*it));
        //auto& file{ordered_files.back().back().get()};
        ordered_files.emplace_back(std::vector<std::string>{});
        ordered_files.back().emplace_back(it->path().string());
        auto& file{*it};
        std::size_t cntr2{cntr + 1};
        for(auto it2{std::next(it)}; it2 != std::end(files); ++it2, ++cntr2)
        {
            auto& file2{*it2};
            if(!processed[cntr2] && file2 == file)
            {
                processed[cntr2] = true;
                //ordered_files.back().emplace_back(std::ref(file2));
                ordered_files.back().emplace_back(file2.path().string());
            }
        }
    }
    return ordered_files;
}


namespace
{

void print(const input_t& input, std::ostream& stream)
{
    const auto& to_discover{input["folders-to-discover"].as<folders_t>()};
    const auto& to_ommit{input["folders-to-ommit"].as<folders_t>()};
    const auto& depth{input["depth"].as<size_type>()};
    const auto& file_size{input["size-of-file"].as<size_type>()};
    //const auto& masks{input["file-masks"].as<masks_t>()};
    const auto& rmasks{input["file-masks"].as<mask_t>()};
    const auto& block_size{input["size-of-block"].as<size_type>()};
    const auto& hash_algo{input["hashf"].as<std::string>()};

    stream << "\nfolders to discover:\n";
    for(const auto& it:to_discover)
        stream << "\t" << std::filesystem::absolute(it) << '\t' << it << '\n';
    stream << "folders to ommit:\n";
    for(const auto& it:to_ommit)
        stream << "\t" << it << '\n';
    stream << "depth of discovering: " << depth << '\n';
    stream << "minimal size of a file to consider in bytes: " << file_size << '\n';

    //stream << "masks of file names to consider:\n";
    //for(const auto& it:masks)
    //    stream << "\t" << it << '\n';
    stream << "masks of file names to consider: " << rmasks << '\n';

    stream << "size of a block to read from file: " << block_size << '\n';
    stream << "hash algorithm: " << hash_algo << '\n';
}

}


ordered_files_t bayan(int argc, const char *argv[])
{
    const auto input{read_cmd_line_args(argc, argv)};

    const auto& to_discover{input["folders-to-discover"].as<folders_t>()};
    const auto& to_ommit{input["folders-to-ommit"].as<folders_t>()};
    const auto& depth{input["depth"].as<size_type>()};
    const auto& file_size{input["size-of-file"].as<size_type>()};
    const auto& rmasks{input["file-masks"].as<mask_t>()};
    const auto& block_size{input["size-of-block"].as<size_type>()};
    const auto& hash_algo{input["hashf"].as<std::string>()};

    //print(input, out_stream);

    auto files{make_set_of_files(to_discover, to_ommit, file_size, rmasks, depth, hash_algo, block_size)};
    return arrange_files(files);
}

