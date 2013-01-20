#include "bacs/single/problem/driver.hpp"
#include "bacs/single/problem/error.hpp"

#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

namespace bacs{namespace single{namespace problem{namespace drivers
{
    class simple0: public driver
    {
    public:
        explicit simple0(const boost::filesystem::path &location);

        api::pb::problem::Problem overview() const override;

        // utilities
        tests_ptr tests() const override;
        utility_ptr checker() const override;
        utility_ptr validator() const override;

        statement_ptr statement() const override;

    private:
        void read_info();
        void read_tests();
        void read_statement();
        void read_profiles();
        void read_checker();
        void read_validator();

    private:
        const boost::filesystem::path m_location;
        boost::property_tree::ptree m_config;
        api::pb::problem::Problem m_overview;
        tests_ptr m_tests;
        utility_ptr m_checker;
        utility_ptr m_validator;
        statement_ptr m_statement;

    private:
        static const bool factory_reg_hook;
    };
}}}}
