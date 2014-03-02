#include "tests.hpp"

#include <bacs/problem/single/driver.hpp>
#include <bacs/problem/single/error.hpp>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

#include <string>
#include <vector>

namespace bacs{namespace problem{namespace single{namespace drivers{
    namespace simple0
{
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
        void read_profiles();
        void read_checker();
        void read_interactor();

    private:
        const boost::filesystem::path m_location;
        boost::property_tree::ptree m_config;
        Problem m_overview;
        tests::shared_ptr<simple0::tests> m_tests;
        utility_ptr m_checker;
        utility_ptr m_interactor;
        statement_ptr m_statement;

    private:
        static const bool factory_reg_hook;
    };
}}}}}
