#include "bacs/single/problem/driver.hpp"
#include "bacs/single/problem/error.hpp"

#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

namespace bacs{namespace single{namespace problem{namespace drivers
{
    struct test_no_in_data_error: virtual test_data_format_error {};
    struct test_no_out_data_error: virtual test_data_format_error {};
    struct test_unknown_data_error: virtual test_data_format_error {};

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
        void read_info(api::pb::problem::Info &info);
        void read_tests(api::pb::problem::Tests &tests);
        void read_statement(api::pb::problem::Statement &statement);
        void read_profiles(google::protobuf::RepeatedPtrField<api::pb::problem::Profile> &profiles);
        void read_utilities(api::pb::problem::Utilities &utilities);

    private:
        const boost::filesystem::path m_location;
        boost::property_tree::ptree m_config;

    private:
        static bool factory_reg_hook;
    };
}}}}
