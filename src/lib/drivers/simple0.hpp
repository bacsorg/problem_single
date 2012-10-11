#include "bacs/single/problem/driver.hpp"

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

        api::pb::problem::Problem overview() override;

        boost::filesystem::path test(const std::string &test_id,
                                     const std::string &data_id) override;

        // utilities
        utility_ptr checker() override;
        utility_ptr validator() override;

        /// \todo this is a stub
        void *statement() override { return nullptr; }

    private:
        void read_tests(api::pb::problem::Tests &tests);

    private:
        const boost::filesystem::path m_location;
        boost::property_tree::ptree m_config;

    private:
        static bool factory_reg_hook;
    };
}}}}
