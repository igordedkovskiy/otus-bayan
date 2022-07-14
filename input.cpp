#include <vector>
#include <string>
#include <filesystem>

#include "input.hpp"
#include "File.hpp"
#include "filter_files.hpp"

input_t read_cmd_line_args(int argc, const char *argv[])
{
    //auto set_bulk = [](size_type bulk)
    //{
    //    std::cout << "bulk size is " << bulk << std::endl;
    //};
    namespace fs = std::filesystem;
    namespace po = boost::program_options;

    po::options_description desc{"Options"};
    desc.add_options()
            ("help,h", "Help page")

            ("folders-to-discover,i",
             po::value<folders_t>()->multitoken()->
             default_value(folders_t(1, File::path_t(".")), "."),
             "folders to discover")

            ("folders-to-ommit,o",
             po::value<folders_t>()->multitoken()->
             default_value(folders_t(), ""),
             "folders to ommit")

            //("depth,d", po::value<size_type>()->default_value(0)->notifier(set_bulk), "depth of discovering")
            ("depth,d", po::value<size_type>()->default_value(0), "depth of discovering")

            ("size-of-file,s", po::value<size_type>()->default_value(1), "minimal size of a file to consider in bytes")

            //("file-masks,m",
            // po::value<masks_t>()->multitoken()->
            // default_value(masks_t(),""),
            // "masks of file names to consider")
            ("file-masks,r", po::value<mask_t>()->default_value(""), "masks of file names to consider")

            ("size-of-block,b", po::value<size_type>()->default_value(5), "size of a block to read from file")

            ("hashf,H", po::value<std::string>()->default_value("std::hash"), "hash algorithm: std:hash, md5, crc32")
            ;

    po::variables_map vm;
    po::store(parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    return vm;
}
