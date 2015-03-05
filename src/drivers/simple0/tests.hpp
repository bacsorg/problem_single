#pragma once

#include <bacs/problem/single/detail/list_tests.hpp>
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

        explicit tests(
            const boost::filesystem::path &location,
            const boost::property_tree::ptree &config);

        std::unordered_set<std::string> data_set() const override
        {
            return m_tests.data_set();
        }

        std::unordered_set<std::string> test_set() const override
        {
            return m_tests.test_set();
        }

        bool make_package(const boost::filesystem::path &destination,
                          const bunsan::pm::entry &package) const
        {
            return m_tests.make_package(destination, package);
        }

    private:
        detail::list_tests m_tests;
    };
}}}}}
