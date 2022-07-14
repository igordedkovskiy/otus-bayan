#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <string>

// https://github.com/stbrumme/hash-library.git
#include "crc32.h"
#include "md5.h"

class Hash
{
public:
    using hash_value_t = std::vector<std::uint8_t>;
    virtual ~Hash() noexcept = 0;
    virtual hash_value_t operator()(const hash_value_t& var) noexcept = 0;
};

class STDHash: public Hash
{
    std::hash<hash_value_t::value_type> m_hasher;
public:
    virtual ~STDHash() noexcept = default;
    hash_value_t operator()(const hash_value_t& var) noexcept override;
};

class MD5Hasher: public Hash
{
    MD5 m_hasher;
public:
    hash_value_t operator()(const hash_value_t& var) noexcept override;
};

class CRC32Hasher: public Hash
{
    CRC32 m_hasher;
public:
    hash_value_t operator()(const hash_value_t& var) noexcept override;
};

enum class HashAlgorithm: std::uint8_t
{
    STD_HASH,
    MD5,
    CRC32
};

std::unique_ptr<Hash> make_hash_algo(HashAlgorithm h);

HashAlgorithm to_hash_algo_name(const std::string& h);
