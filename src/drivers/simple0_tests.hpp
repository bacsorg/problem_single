#pragma once

#include <bacs/problem/single/detail/list_tests.hpp>
#include <bacs/problem/single/error.hpp>

namespace bacs{namespace problem{namespace single{namespace drivers
{
    struct test_no_in_data_error: virtual test_data_format_error {};
    struct test_no_out_data_error: virtual test_data_format_error {};
    struct test_unknown_data_error: virtual test_data_format_error {};

    // implementation without registration in factory
    class simple0_tests: public detail::list_tests
    {
    public:
        simple0_tests(const boost::filesystem::path &location,
                      const std::unordered_set<std::string> &text_data_set);
    };
}}}}
