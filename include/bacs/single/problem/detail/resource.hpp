#pragma once

#include "bacs/single/problem/error.hpp"

#include <string>

namespace bacs{namespace single{namespace problem{namespace detail
{
    struct resource_format_error: virtual error
    {
        typedef boost::error_info<struct tag_value, std::string> value;
        typedef boost::error_info<struct tag_pos, std::size_t> pos;
    };

    struct time_format_error: virtual resource_format_error {};
    struct memory_format_error: virtual resource_format_error {};

    std::uint64_t parse_time_millis(const std::string &time);
    std::uint64_t parse_memory_bytes(const std::string &memory);
}}}}
