#include "driver.hpp"

#include <bacs/problem/single/detail/path.hpp>
#include <bacs/problem/single/error.hpp>
#include <bacs/problem/single/problem.pb.h>

#include <bacs/problem/split.hpp>
#include <bacs/problem/resource/parse.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

namespace bacs{namespace problem{namespace single{namespace drivers{
    namespace simple0
{
    const bool driver::factory_reg_hook = driver::register_new("simple0",
        [](const boost::filesystem::path &location)
        {
            driver_ptr tmp(new driver(location));
            return tmp;
        });

    driver::driver(const boost::filesystem::path &location):
        m_location(location)
    {
        boost::property_tree::read_ini((location / "config.ini").string(), m_config);
        // TODO
        read_info();
        read_tests();
        read_statement();
        read_profiles();
        read_checker();
        read_interactor();
        // depending on tests set test order may differ
        // this code should be executed after profiles and tests initialization
        BOOST_ASSERT(m_overview.profile_size() == 1);
        BOOST_ASSERT(
            m_overview.profile(0).GetExtension(Profile_::testing).test_group_size() == 1);
        // select tests order
        bool only_digits = true;
        for (const std::string &id: m_overview.GetExtension(Problem_::tests).test_set())
        {
            if (!(only_digits = only_digits &&
                  std::all_of(id.begin(), id.end(), boost::algorithm::is_digit())))
                break;
        }
        m_overview.mutable_profile(0)->MutableExtension(Profile_::testing)->
            mutable_test_group(0)->mutable_settings()->mutable_run()->set_order(
                only_digits ? settings::Run::NUMERIC : settings::Run::LEXICOGRAPHICAL);
    }

    Problem driver::overview() const
    {
        return m_overview;
    }

    // utilities
    tests_ptr driver::tests() const
    {
        return m_tests;
    }

    utility_ptr driver::checker() const
    {
        return m_checker;
    }

    utility_ptr driver::interactor() const
    {
        return m_interactor;
    }

    statement_ptr driver::statement() const
    {
        return m_statement;
    }

    void driver::read_info()
    {
        Info &info = *m_overview.mutable_info();
        info.Clear();
        const boost::property_tree::ptree m_info = m_config.get_child("info");
        // name
        Info::Name &name = *info.add_name();
        name.set_lang("C");
        name.set_value(m_info.get<std::string>("name"));
        // authors
        split::parse_repeated(*info.mutable_author(), m_info, "authors");
        // source
        boost::optional<std::string> value;
        if ((value = m_info.get_optional<std::string>("source")))
            info.set_source(value.get());
        // maintainers
        split::parse_repeated(*info.mutable_maintainer(), m_info, "maintainers");
        // system
        Info::System &system = *info.mutable_system();
        system.set_package("unknown"); // initialized later
        system.set_hash("unknown"); // initialized later
    }

    void driver::read_tests()
    {
        m_tests.reset(new simple0::tests(m_location / "tests"));
        const boost::optional<boost::property_tree::ptree &> tests_ =
            m_config.get_child_optional("tests");
        if (tests_)
        {
            for (const auto kv: tests_.get())
            {
                const std::string data_type = kv.second.get_value<std::string>();
                m_tests->set_data_type(
                    kv.first,
                    boost::lexical_cast<tests::test_data_type>(data_type));
            }
        }
        *m_overview.MutableExtension(Problem_::tests) = m_tests->test_set_info();
        *m_overview.mutable_utilities()->MutableExtension(Utilities_::tests) = m_tests->info();
    }

    void driver::read_statement()
    {
        m_statement = statement::instance(m_location / "statement");
        Statement &info = *m_overview.mutable_statement() = m_statement->info();
        for (Statement::Version &v: *info.mutable_version())
        {
            const bunsan::pm::entry package = bunsan::pm::entry("statement") / v.package();
            v.set_package(package.name());
        }
    }

    void driver::read_profiles()
    {
        google::protobuf::RepeatedPtrField<Profile> &profiles = *m_overview.mutable_profile();
        profiles.Clear();
        Profile &profile = *profiles.Add();
        testing::SolutionTesting &testing = *profile.MutableExtension(Profile_::testing);
        testing.Clear();
        testing::TestGroup &test_group = *testing.add_test_group();
        test_group.set_id("");
        settings::TestGroupSettings &settings = *test_group.mutable_settings();
        settings::ProcessSettings &process = *settings.mutable_process();
        {
            boost::optional<std::string> value;
            // resource limits
            ResourceLimits &resource_limits = *process.mutable_resource_limits();
            if ((value = m_config.get_optional<std::string>("resource_limits.time")))
                resource_limits.set_time_limit_millis(
                    resource::parse::time_millis(value.get()));
            if ((value = m_config.get_optional<std::string>("resource_limits.memory")))
                resource_limits.set_memory_limit_bytes(
                    resource::parse::memory_bytes(value.get()));
            if ((value = m_config.get_optional<std::string>("resource_limits.output")))
                resource_limits.set_output_limit_bytes(
                    resource::parse::memory_bytes(value.get()));
            // number of processes is not supported here
            if ((value = m_config.get_optional<std::string>("resource_limits.real_time")))
                resource_limits.set_real_time_limit_millis(
                    resource::parse::time_millis(value.get()));
            // run
            settings::Run &run = *settings.mutable_run();
            //run.set_order(); // depending on tests, is set in other location
            run.set_algorithm(settings::Run::WHILE_NOT_FAIL);
            // files & execution
            settings::File *file = process.add_file();
            settings::Execution &execution = *process.mutable_execution();
            file->set_id("stdin");
            file->set_init("in");
            file->add_permission(settings::File::READ);
            if ((value = m_config.get_optional<std::string>("files.stdin")))
            {
                detail::to_pb_path(value.get(), *file->mutable_path());
            }
            else
            {
                settings::Execution::Redirection &rd = *execution.add_redirection();
                rd.set_stream(settings::Execution::Redirection::STDIN);
                rd.set_file_id("stdin");
            }
            file = process.add_file();
            file->set_id("stdout");
            file->add_permission(settings::File::READ);
            file->add_permission(settings::File::WRITE);
            if ((value = m_config.get_optional<std::string>("files.stdout")))
            {
                detail::to_pb_path(value.get(), *file->mutable_path());
            }
            else
            {
                settings::Execution::Redirection &rd = *execution.add_redirection();
                rd.set_stream(settings::Execution::Redirection::STDOUT);
                rd.set_file_id("stdout");
            }
        }
        test_group.clear_test_set();
        testing::TestQuery &test_query = *test_group.add_test_set();
        test_query.mutable_wildcard()->set_value("*"); // select all tests
    }

    void driver::read_checker()
    {
        m_checker = utility::instance_optional(m_location / "checker");
        if (!m_checker)
            BOOST_THROW_EXCEPTION(
                checker_error() <<
                checker_error::message("Unable to initialize checker's driver."));
        *m_overview.mutable_utilities()->MutableExtension(Utilities_::checker) =
            m_checker->info();
    }

    void driver::read_interactor()
    {
        if (boost::filesystem::exists(m_location / "interactor"))
        {
            m_interactor = utility::instance_optional(m_location / "interactor");
            if (!m_interactor)
                BOOST_THROW_EXCEPTION(
                    interactor_error() <<
                    interactor_error::message("Unable to initialize interactor's driver."));
            *m_overview.mutable_utilities()->MutableExtension(Utilities_::interactor) =
                m_interactor->info();
        }
    }
}}}}}
