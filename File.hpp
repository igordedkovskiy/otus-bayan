#pragma once

#include <memory>
#include <filesystem>
#include "hash.hpp"

// TODO: change hash_value_t: hash_value_t = std::vector<uint64_t>;
// TODO: pack Hash::hash_value_t into File::hash_value_t - less elements to compare
class File
{
public:
    using path_t = std::filesystem::path;
    using size_type = decltype(std::filesystem::file_size(path_t{}));
    using hash_value_t = Hash::hash_value_t;
    using file_hash_t = std::vector<hash_value_t>;

    File(const File&) = delete;
    File(File&&) noexcept = default;

    File& operator=(const File&) = delete;
    File& operator=(File&&) noexcept = delete;

    File(path_t&& path, HashAlgorithm h = HashAlgorithm::STD_HASH);

    static void block_size(size_type block_size) noexcept;

    const path_t& path() const noexcept;

    size_type size() const;

    size_type size_hashed() const noexcept;

    const file_hash_t& hash() const noexcept;

    const hash_value_t& last_block_of_hash() const noexcept;

    const file_hash_t& update_hash();

    friend bool operator==(File& l, File& r);

private:
    static size_type block_size() noexcept;

    const path_t m_path;
    std::unique_ptr<Hash> m_hash_algo{std::make_unique<STDHash>(STDHash())};
    file_hash_t m_hash{file_hash_t{}};
    size_type m_size{std::filesystem::file_size(m_path)};
    static size_type m_block_size;
};


