#pragma once

#include <string>

namespace bacs{namespace single{namespace problem{namespace detail
{
    std::uint64_t parse_time_millis(const std::string &time);
    std::uint64_t parse_memory_bytes(const std::string &memory);
}}}}
