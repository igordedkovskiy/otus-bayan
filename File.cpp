#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <cmath>

#include "File.hpp"

File::File(path_t &&path, HashAlgorithm h):
    m_path{std::move(path)},
    m_hash_algo{make_hash_algo(h)}
{}

File::size_type File::m_block_size = 1;

const File::path_t &File::path() const noexcept
{
    return m_path;
}

File::size_type File::size() const
{
    return m_size;
}

File::size_type File::size_hashed() const noexcept
{
    return m_hash.size() * m_block_size;
}

void File::block_size(size_type block_size) noexcept
{
    m_block_size = block_size ? block_size : 1;
}

File::size_type File::block_size() noexcept
{
    return m_block_size;
}

const File::file_hash_t& File::hash() const noexcept
{
    return m_hash;
}

const File::hash_value_t& File::last_block_of_hash() const noexcept
{
    return m_hash.back();
}

const File::file_hash_t& File::update_hash()
{
    const auto sz_hashed{size_hashed()};
    if(m_size && sz_hashed == std::ceil(m_size / m_block_size))
        return m_hash;

    const auto size_to_read{std::min(m_block_size, m_size - sz_hashed)};
    std::ifstream fs(m_path.c_str(), std::fstream::in | std::ios::binary);
    if(fs.is_open())
    {
        fs.seekg(sz_hashed);
        hash_value_t block(size_to_read, 0);
        fs.read(reinterpret_cast<char*>(block.data()), size_to_read);
        fs.close();
        m_hash.emplace_back((*m_hash_algo)(block));
    }
    return m_hash;
}


/// \brief Compare files
/// \details Files are equal if hash sums of it's content are equal
bool operator==(File& l, File& r)
{
    if(l.size() != r.size())
        return false;

    if(!l.size_hashed())
        l.update_hash();
    if(l.size_hashed() > r.size_hashed())
    {
        while(l.size_hashed() != r.size_hashed())
            r.update_hash();
    }
    else if(l.size_hashed() < r.size_hashed())
    {
        while(l.size_hashed() != r.size_hashed())
            l.update_hash();
    }

    if(l.hash() != r.hash())
        return false;

    while(l.size_hashed() < l.size())
    {
        l.update_hash();
        r.update_hash();
        if(l.last_block_of_hash() != r.last_block_of_hash())
            return false;
    }
    return true;
}


