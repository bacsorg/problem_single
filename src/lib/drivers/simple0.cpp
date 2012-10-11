#include "simple0.hpp"

#include "bacs/single/problem/error.hpp"

#include <unordered_set>
#include <unordered_map>

#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace bacs{namespace single{namespace problem{namespace drivers
{
    bool simple0::factory_reg_hook = driver::register_new("simple0",
        [](const boost::filesystem::path &location)
        {
            driver_ptr tmp(new simple0(location));
            return tmp;
        });

    simple0::simple0(const boost::filesystem::path &location):
        m_location(location)
    {
        boost::property_tree::read_ini((location / "config.ini").string(), m_config);
    }

    api::pb::problem::Problem simple0::overview()
    {
        api::pb::problem::Problem problem;
        read_info(*problem.mutable_info());
        read_tests(*problem.mutable_tests());
        read_statement(*problem.mutable_statement());
        read_profiles(*problem.mutable_profiles());
        read_utilities(*problem.mutable_utilities());
        return problem;
    }

    boost::filesystem::path simple0::test(const std::string &test_id,
                                          const std::string &data_id)
    {
        if (test_id.find('.') != std::string::npos)
            BOOST_THROW_EXCEPTION(error() << error::message("Invalid test id."));
        if (data_id.find('.') != std::string::npos)
            BOOST_THROW_EXCEPTION(error() << error::message("Invalid data id."));
        return m_location / (test_id + "." + data_id);
    }

    // utilities
    utility_ptr simple0::checker()
    {
        return utility::instance(m_location / "checker");
    }

    utility_ptr simple0::validator()
    {
        return utility::instance(m_location / "checker");
    }

    //void *simple0::statement() { return nullptr; }

    void simple0::read_info(api::pb::problem::Info &info)
    {
    }

    void simple0::read_tests(api::pb::problem::Tests &tests)
    {
        const boost::filesystem::directory_iterator end;
        std::unordered_set<std::string> test_set;
        std::unordered_set<std::string> data_set;
        std::unordered_map<std::string, std::unordered_set<std::string>> test_files;
        for (boost::filesystem::directory_iterator i(m_location / "tests"); i != end; ++i)
        {
            if (!boost::filesystem::is_regular_file(i->path()))
                BOOST_THROW_EXCEPTION(error() << error::message("Invalid tests format."));
            const std::string fname = i->path().filename().string();
            const std::string::size_type dot = fname.find('.');
            if (dot == std::string::npos)
                BOOST_THROW_EXCEPTION(error() << error::message("Invalid tests format."));
            if (fname.find('.', dot + 1) != std::string::npos)
                BOOST_THROW_EXCEPTION(error() << error::message("Invalid tests format."));
            const std::string test_id = fname.substr(0, dot);
            const std::string data_id = fname.substr(dot + 1);
            test_set.insert(test_id);
            data_set.insert(data_id);
            test_files[test_id].insert(data_id);
        }
        for (const auto &test_info: test_files)
            if (test_info.second != data_set)
                BOOST_THROW_EXCEPTION(error() << error::message("Invalid tests format."));
        tests.Clear();
        for (const std::string &test_id: test_set)
            tests.add_test_set(test_id);
        for (const std::string &data_id: data_set)
        {
            api::pb::problem::Tests::Data &data = *tests.add_data_set();
            data.set_id(data_id);
            const std::string format = m_config.get<std::string>("tests." + data_id, "text");
            if (format == "text")
                data.set_format(api::pb::problem::Tests::Data::TEXT);
            else if (format == "binary")
                data.set_format(api::pb::problem::Tests::Data::BINARY);
            else
                BOOST_THROW_EXCEPTION(error() << error::message("Invalid data format."));
        }
    }

    void simple0::read_statement(api::pb::problem::Statement &statement)
    {
        // TODO
    }

    void simple0::read_profiles(google::protobuf::RepeatedPtrField<api::pb::problem::Profile> &profiles)
    {
        profiles.Clear();
        api::pb::problem::Profile &profile = *profiles.Add();
        api::pb::testing::SolutionTesting &testing = *profile.mutable_testing();
        testing.Clear();
        api::pb::testing::TestGroup &test_group = *testing.add_test_groups();
        api::pb::settings::TestGroupSettings &settings = *test_group.mutable_settings();
        {
            // TODO fill settings
        }
        test_group.clear_test_set();
        api::pb::testing::TestQuery &test_query = *test_group.add_test_set();
        test_query.set_wildcard("*"); // select all tests
    }

    void simple0::read_utilities(api::pb::problem::Utilities &utilities)
    {
        // TODO
    }
}}}}
