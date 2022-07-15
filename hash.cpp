#include <array>
#include "hash.hpp"

Hash::~Hash() noexcept {}

Hash::hash_value_t STDHash::operator()(const hash_value_t& var) noexcept
{
    hash_value_t result;
    result.reserve(var.size());
    for(const auto val:var)
        result.emplace_back(m_hasher(val));
    return result;
}

Hash::hash_value_t MD5Hasher::operator()(const hash_value_t& var) noexcept
{
    //return Hash::operator()(var);
    m_hasher.add(var.data(), var.size());
    std::array<unsigned char, MD5::HashBytes> buffer;
    m_hasher.getHash(buffer.data());
    return hash_value_t{std::begin(buffer), std::end(buffer)};
}

Hash::hash_value_t CRC32Hasher::operator()(const hash_value_t& var) noexcept
{
    //return Hash::operator()(var);
    m_hasher.add(var.data(), var.size());
    std::array<unsigned char, CRC32::HashBytes> buffer;
    m_hasher.getHash(buffer.data());
    return hash_value_t{std::begin(buffer), std::end(buffer)};
}

std::unique_ptr<Hash> make_hash_algo(HashAlgorithm h)
{
    if(h == HashAlgorithm::STD_HASH)
        return std::make_unique<STDHash>();
    if(h == HashAlgorithm::MD5)
        return std::make_unique<MD5Hasher>();
    return std::make_unique<CRC32Hasher>();
}

HashAlgorithm to_hash_algo_name(const std::string& h)
{
    if(h == "crc32")
        return HashAlgorithm::CRC32;
    if(h == "md5")
        return HashAlgorithm::MD5;
    return HashAlgorithm::STD_HASH;
}

