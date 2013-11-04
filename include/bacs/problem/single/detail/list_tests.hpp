#pragma once

#include <bacs/problem/single/error.hpp>
#include <bacs/problem/single/tests.hpp>

#include <bunsan/stream_enum.hpp>

#include <unordered_map>

namespace bacs{namespace problem{namespace single{namespace detail
{
    struct inconsistent_test_data_error: virtual tests_error {};
    struct inconsistent_test_data_set_error: virtual inconsistent_test_data_error {};
    struct test_empty_set_error: virtual tests_error {};
    struct test_unknown_data_error: virtual tests_error {};

    class list_tests: public tests
    {
    public:
        typedef std::unordered_map<std::string, boost::filesystem::path> test_data;

        BUNSAN_INCLASS_STREAM_ENUM_CLASS(test_data_type,
        (
            text,
            binary
        ))

    public:
        list_tests(const boost::filesystem::path &location,
                   const test_data_type default_data_type);

        void add_test(const std::string &test_id,
                      const test_data &data);

        test_data_type data_type(const std::string &data_id) const;
        void set_data_type(const std::string &data_id, const test_data_type type);

        std::unordered_set<std::string> data_set() const override;
        std::unordered_set<std::string> test_set() const override;

        bool make_package(const boost::filesystem::path &destination,
                          const bunsan::pm::entry &package) const override;

    protected:
        list_tests(const boost::filesystem::path &location,
                   const test_data_type default_data_type,
                   const std::string &builder_name);

    private:
        const test_data_type m_default_data_type;
        std::unordered_map<std::string, test_data> m_tests;
        std::unordered_set<std::string> m_text_data_set;
    };
}}}}
