#pragma once

#include "bacs/single/problem/tests.hpp"
#include "bacs/single/problem/error.hpp"

namespace bacs{namespace single{namespace problem{namespace drivers
{
    struct test_no_in_data_error: virtual test_data_format_error {};
    struct test_no_out_data_error: virtual test_data_format_error {};
    struct test_unknown_data_error: virtual test_data_format_error {};

    // implementation without registration in factory
    class simple0_tests: public tests
    {
    public:
        simple0_tests(const boost::filesystem::path &location,
                      const std::unordered_set<std::string> &text_data_set);

        bool make_package(const boost::filesystem::path &destination,
                          const bunsan::pm::entry &package) override;

        std::unordered_set<std::string> data_set() override;
        std::unordered_set<std::string> test_set() override;

    private:
        std::unordered_set<std::string> m_data_set, m_test_set;
        const std::unordered_set<std::string> m_text_data_set;
    };
}}}}
