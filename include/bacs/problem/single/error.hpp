#pragma once

#include <bacs/problem/error.hpp>

namespace bacs{namespace problem{namespace single
{
    struct error: virtual bacs::problem::error {};
    struct invalid_id_error: virtual error {};
    struct driver_error: virtual error {};
    struct utility_error: virtual driver_error {};
    struct checker_error: virtual utility_error {};
    struct validator_error: virtual utility_error {};
    struct test_error: virtual error {};
    struct invalid_test_id_error: virtual invalid_id_error, virtual test_error {};
    struct invalid_data_id_error: virtual invalid_id_error, virtual test_error {};
    struct test_format_error: virtual error {};

    struct test_data_format_error: virtual test_format_error
    {
        typedef boost::error_info<struct tag_data_id, std::string> data_id;
    };
}}}
