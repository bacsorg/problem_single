#include "driver.hpp"

#include "statement.hpp"

#include <bacs/problem/single/detail/path.hpp>
#include <bacs/problem/single/problem.pb.h>
#include <bacs/problem/split.hpp>

#include <bunsan/static_initializer.hpp>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace bacs{namespace problem{namespace single{namespace drivers{
    namespace polygon_codeforces_com
{
    BUNSAN_STATIC_INITIALIZER(bacs_problem_single_drivers_polygon_codeforces_com,
    {
        BUNSAN_FACTORY_REGISTER(polygon_codeforces_com, single::driver, "polygon.codeforces.com",
            [](const boost::filesystem::path &location)
            {
                driver_ptr tmp(new driver(location));
                return tmp;
            })
    })

    driver::driver(
        const boost::filesystem::path &location):
            m_location(location)
    {
        boost::property_tree::read_xml(
            (m_location / "problem.xml").string(), m_config);
        boost::property_tree::read_ini(
            (m_location / "config.ini").string(), m_override_config);
        read_info();
        read_statement();
        read_profiles();
        read_tests(); // should be called after read_profiles()
        read_checker();
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
        return utility_ptr(); // not supported
    }

    statement_ptr driver::statement() const
    {
        return m_statement;
    }

    void driver::read_info()
    {
        Info &info = *m_overview.mutable_info();
        info.Clear();
        for (const boost::property_tree::ptree::value_type &name_value:
                 m_config.get_child("problem.names"))
        {
            if (name_value.first == "<xmlattr>")
                continue;
            Info::Name &name = *info.add_name();
            name.set_lang(name_value.second.get<std::string>(
                "<xmlattr>.language", "C"));
            name.set_value(name_value.second.get<std::string>(
                "<xmlattr>.value", "UNNAMED"));
        }
        const boost::property_tree::ptree m_info = m_override_config.get_child("info");
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
        BOOST_ASSERT_MSG(m_tests, "Initialized by driver::read_profiles()");
        // note: binary tests are not supported for now
        *m_overview.MutableExtension(Problem_::tests) =
            m_tests->test_set_info();
        *m_overview.mutable_utilities()->MutableExtension(Utilities_::tests) =
            m_tests->info();
    }

    void driver::read_statement()
    {
        m_statement.reset(new polygon_codeforces_com::statement(
            m_location / "statements",
            m_config.get_child("problem.statements")));
        Statement &info = *m_overview.mutable_statement() = m_statement->info();
        for (Statement::Version &v: *info.mutable_version())
        {
            const bunsan::pm::entry package =
                bunsan::pm::entry("statement") / v.package();
            v.set_package(package.name());
        }
    }

    void driver::read_profiles()
    {
        m_tests.reset(new polygon_codeforces_com::tests(m_location));
        google::protobuf::RepeatedPtrField<Profile> &profiles = *m_overview.mutable_profile();
        profiles.Clear();
        Profile &profile = *profiles.Add();
        testing::SolutionTesting &testing = *profile.MutableExtension(Profile_::testing);
        testing.Clear();
        const boost::property_tree::ptree judging = m_config.get_child("problem.judging");
        for (const boost::property_tree::ptree::value_type &testset: judging)
        {
            if (testset.first == "<xmlattr>")
                continue;
            testing::TestGroup &test_group = *testing.add_test_group();
            test_group.set_id(testset.second.get<std::string>("<xmlattr>.name", ""));
            settings::TestGroupSettings &settings = *test_group.mutable_settings();
            settings::ProcessSettings &process = *settings.mutable_process();
            { // resource limits
                boost::optional<std::int64_t> value;
                bacs::process::ResourceLimits &resource_limits =
                    *process.mutable_resource_limits();
                if ((value = testset.second.get_optional<std::int64_t>("time-limit")))
                    resource_limits.set_time_limit_millis(*value);
                if ((value = testset.second.get_optional<std::int64_t>("memory-limit")))
                    resource_limits.set_memory_limit_bytes(*value);
                // output limit is not supported here
                // number of processes is not supported here
                // real time limit is not supported here
            }
            { // run
                boost::optional<std::string> value;
                settings::Run &run = *settings.mutable_run();
                run.set_order(settings::Run::NUMERIC);
                run.set_algorithm(settings::Run::WHILE_NOT_FAIL);
                // files & execution
                settings::File *file = process.add_file();
                settings::Execution &execution = *process.mutable_execution();
                file->set_id("stdin");
                file->set_init("in");
                file->add_permission(settings::File::READ);
                if ((value = judging.get_optional<std::string>("<xmlattr>.input-file")) &&
                    !value->empty())
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
                if ((value = judging.get_optional<std::string>("<xmlattr>.output-file")) &&
                    !value->empty())
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
            const std::string in_test_format =
                testset.second.get<std::string>("input-path-pattern", "tests/%02d");
            const std::string out_test_format =
                testset.second.get<std::string>("output-path-pattern", "tests/%02d.a");
            test_group.clear_test_set();
            {
                std::size_t test_id_ = 0;
                for (const boost::property_tree::ptree::value_type &test:
                     testset.second.get_child("tests"))
                {
                    if (test.first == "<xmlattr>")
                        continue;
                    const std::string test_id = boost::lexical_cast<std::string>(++test_id_);
                    testing::TestQuery &test_query = *test_group.add_test_set();
                    test_query.set_id(test_id);
                    m_tests->add_test(
                        test_id,
                        str(boost::format(in_test_format) % test_id),
                        str(boost::format(out_test_format) % test_id)
                    );
                }
            }
        }
    }

    namespace
    {
        utility_ptr get_utility(
            const boost::property_tree::ptree &config,
            const boost::filesystem::path &root,
            const std::string &target,
            const std::string &call,
            const std::string &return_)
        {
            const boost::filesystem::path path =
                config.get<std::string>("source.<xmlattr>.path");
            boost::property_tree::ptree cfg;
            cfg.put("build.builder", "single");
            cfg.put("build.target", target);
            cfg.put("build.source", path.filename().string());
            cfg.put("utility.call", call);
            cfg.put("utility.return", return_);
            return utility::instance(root / path.parent_path(), cfg);
        }

        utility_ptr get_utility_default(
            const boost::filesystem::path &root,
            const std::string &target,
            const std::string &call)
        {
            boost::property_tree::ptree cfg;
            cfg.put("build.target", target);
            cfg.put("utility.call", call);
            return utility::instance(root, cfg);
        }
    }

    void driver::read_checker()
    {
        const boost::optional<boost::property_tree::ptree &> checker =
            m_config.get_child_optional("problem.assets.checker");
        if (checker)
            m_checker = get_utility(*checker, m_location, "checker", "in_out_hint", "testlib");
        else
            m_checker = get_utility_default(m_location, "checker", "std/strict/out_stdout");
        *m_overview.mutable_utilities()->MutableExtension(Utilities_::checker) =
            m_checker->info();
    }
}}}}}
