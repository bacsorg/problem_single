#pragma once

#include <bacs/problem/single/error.hpp>
#include <bacs/problem/single/problem.pb.h>
#include <bacs/problem/utility.hpp>

#include <bunsan/factory_helper.hpp>
#include <bunsan/forward_constructor.hpp>

#include <unordered_set>

namespace bacs{namespace problem{namespace single
{
    struct tests_error: virtual error
    {
        typedef boost::error_info<struct tag_test_id, std::string> test_id;
        typedef boost::error_info<struct tag_data_id, std::string> data_id;
    };
    struct invalid_test_id_error: virtual invalid_id_error, virtual tests_error {};
    struct invalid_data_id_error: virtual invalid_id_error, virtual tests_error {};
    struct test_format_error: virtual tests_error {};
    struct test_data_format_error: virtual test_format_error {};
    struct tests_make_package_error: virtual utility_make_package_error, virtual tests_error {};

    class tests: public utility
    BUNSAN_FACTORY_BEGIN(tests, const boost::filesystem::path &/*location*/,
                                const boost::property_tree::ptree &/*config*/)
    public:
        static tests_ptr instance(const boost::filesystem::path &location);

        BUNSAN_FORWARD_EXPLICIT_CONSTRUCTOR(tests, utility)

    public:
        // create() will be provided by appropriate package
        virtual std::unordered_set<std::string> data_set() const=0;
        virtual std::unordered_set<std::string> test_set() const=0;

        Tests test_set_info() const;

    BUNSAN_FACTORY_END(tests)
}}}
