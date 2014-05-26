#pragma once

#include <bacs/problem/single/error.hpp>
#include <bacs/problem/single/tests.hpp>

namespace bacs{namespace problem{namespace single{namespace drivers{
    namespace simple0
{
    struct test_no_in_data_error: virtual test_data_format_error {};
    struct test_unknown_data_error: virtual test_data_format_error {};

    class tests: public single::tests
    {
    public:
        using single::tests::tests;

    public:
        using single::tests::instance;

        static tests_ptr instance(
            const boost::filesystem::path &location,
            const boost::property_tree::ptree &config);
    };
}}}}}
