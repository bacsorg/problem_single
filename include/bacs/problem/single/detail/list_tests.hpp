#pragma once

#include <bacs/problem/single/error.hpp>
#include <bacs/problem/single/tests.hpp>

#include <unordered_map>

namespace bacs{namespace problem{namespace single{namespace detail
{
    struct inconsistent_test_data_error: virtual test_error {};
    struct inconsistent_test_data_set_error: virtual inconsistent_test_data_error {};
    struct test_empty_set_error: virtual test_error {};
    struct test_unknown_data_error: virtual test_error {};

    class list_tests: public tests
    {
    public:
        typedef std::unordered_map<std::string, boost::filesystem::path> test_data;

    public:
        explicit list_tests(const boost::filesystem::path &location);
        void add_test(const std::string &test_id,
                      const test_data &data);

        /// \note By default all data is treated like text
        void text_data(const std::string &data_id, const bool value=true);

        std::unordered_set<std::string> data_set() const override;
        std::unordered_set<std::string> test_set() const override;

        bool make_package(const boost::filesystem::path &destination,
                          const bunsan::pm::entry &package) const override;

    protected:
        list_tests(const boost::filesystem::path &location,
                   const std::string &builder_name);

    private:
        std::unordered_map<std::string, test_data> m_tests;
        std::unordered_set<std::string> m_text_data_set;
    };
}}}}
