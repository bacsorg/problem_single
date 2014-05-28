#pragma once

#include <bacs/problem/single/driver.hpp>
#include <bacs/problem/single/error.hpp>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

#include <map>
#include <string>
#include <vector>

namespace bacs{namespace problem{namespace single{namespace drivers{
    namespace simple0
{
    struct invalid_group_id_error: virtual error {};
    struct empty_group_id_error: virtual invalid_group_id_error {};

    /// \ref format_simple0_page implementation
    class driver: public single::driver
    {
    public:
        explicit driver(const boost::filesystem::path &location);

        Problem overview() const override;

        // utilities
        tests_ptr tests() const override;
        utility_ptr checker() const override;
        utility_ptr interactor() const override;

        statement_ptr statement() const override;

    private:
        void read_info();
        void read_tests();
        void read_statement();
        void read_settings();
        void read_profiles();
        void read_checker();
        void read_interactor();

    private:
        const boost::filesystem::path m_location;
        boost::property_tree::ptree m_config;
        settings::TestGroupSettings m_settings;
        Problem m_overview;
        tests_ptr m_tests;
        utility_ptr m_checker;
        utility_ptr m_interactor;
        statement_ptr m_statement;

        // sorted
        std::map<
            std::string,
            std::vector<std::string>
        > m_test_groups;

    private:
        static const bool factory_reg_hook;
    };
}}}}}
