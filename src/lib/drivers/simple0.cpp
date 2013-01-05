#include "simple0.hpp"
#include "simple0_tests.hpp"

#include "bacs/single/problem/error.hpp"

#include "bacs/single/problem/detail/split.hpp"
#include "bacs/single/problem/detail/resource.hpp"
#include "bacs/single/problem/detail/path.hpp"

#include <algorithm>
#include <unordered_set>
#include <unordered_map>

#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/assert.hpp>

namespace bacs{namespace single{namespace problem{namespace drivers
{
    const bool simple0::factory_reg_hook = driver::register_new("simple0",
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
        BOOST_ASSERT(problem.profiles_size() == 1);
        BOOST_ASSERT(problem.mutable_profiles(0)->mutable_testing()->test_groups_size() == 1);
        bool only_digits = true;
        for (const std::string &id: problem.tests().test_set())
        {
            if (!(only_digits = only_digits && std::all_of(id.begin(), id.end(), boost::algorithm::is_digit())))
                break;
        }
        problem.mutable_profiles(0)->mutable_testing()->mutable_test_groups(0)->
            mutable_settings()->mutable_run()->set_order(
                only_digits ? api::pb::settings::Run::NUMERIC : api::pb::settings::Run::LEXICOGRAPHICAL);
        return problem;
    }

    // utilities
    tests_ptr simple0::tests()
    {
        const boost::optional<boost::property_tree::ptree &> tests_ =
            m_config.get_child_optional("tests");
        std::unordered_set<std::string> text_data_set = {"in", "out"};
        if (tests_)
            for (const auto kv: tests_.get())
            {
                const std::string data_type = kv.second.get_value<std::string>();
                BOOST_ASSERT(kv.first == "in" || kv.first == "out");
                if (data_type == "binary")
                    text_data_set.erase(kv.first);
                else if (data_type == "text")
                    text_data_set.insert(kv.first);
                else
                    BOOST_THROW_EXCEPTION(test_data_format_error() <<
                                          test_data_format_error::data_id(kv.first));
            }
        const tests_ptr tmp(new simple0_tests(m_location / "tests", text_data_set));
        return tmp;
    }

    utility_ptr simple0::checker()
    {
        const utility_ptr checker_ = utility::instance_optional(m_location / "checker");
        if (!checker_)
            BOOST_THROW_EXCEPTION(checker_error() <<
                                  checker_error::message("Unable to initialize checker's driver."));
        return checker_;
    }

    utility_ptr simple0::validator()
    {
        if (boost::filesystem::exists(m_location / "validator"))
        {
            const utility_ptr validator_ = utility::instance_optional(m_location / "validator");
            if (!validator_)
                BOOST_THROW_EXCEPTION(validator_error() <<
                                      validator_error::message("Unable to initialize validator's driver."));
            return validator_;
        }
        else
        {
            return utility_ptr();
        }
    }

    //void *simple0::statement() { return nullptr; }

    void simple0::read_info(api::pb::problem::Info &info)
    {
        info.Clear();
        const boost::property_tree::ptree m_info = m_config.get_child("info");
        // name
        api::pb::problem::Info::Name &name = *info.add_names();
        name.set_lang("C");
        name.set_value(m_info.get<std::string>("name"));
        // authors
        detail::parse_repeated(*info.mutable_authors(), m_info, "authors");
        // source
        boost::optional<std::string> value;
        if ((value = m_info.get_optional<std::string>("source")))
            info.set_source(value.get());
        // maintainers
        detail::parse_repeated(*info.mutable_maintainers(), m_info, "maintainers");
        // restrictions
        detail::parse_repeated(*info.mutable_restrictions(), m_info, "restrictions");
        // system
        api::pb::problem::Info::System &system = *info.mutable_system();
        system.set_package("unknown"); // is set by generator
        system.set_hash("TODO"); // TODO implement hashing
    }

    void simple0::read_tests(api::pb::problem::Tests &tests__)
    {
        const tests_ptr tests_ = tests();
        BOOST_ASSERT(tests_);
        tests__.Clear();
        for (const std::string &data_id: tests_->data_set())
            tests__.add_data_set(data_id);
        for (const std::string &test_id: tests_->test_set())
            tests__.add_test_set(test_id);
    }

    void simple0::read_statement(api::pb::problem::Statement &statement)
    {
        for (boost::filesystem::directory_iterator i(m_location / "statement"), end; i != end; ++i)
        {
            if (i->path().filename().extension() == ".ini" &&
                boost::filesystem::is_regular_file(i->path()))
            {
                // TODO
            }
        }
    }

    void simple0::read_profiles(google::protobuf::RepeatedPtrField<api::pb::problem::Profile> &profiles)
    {
        profiles.Clear();
        api::pb::problem::Profile &profile = *profiles.Add();
        api::pb::testing::SolutionTesting &testing = *profile.mutable_testing();
        testing.Clear();
        api::pb::testing::TestGroup &test_group = *testing.add_test_groups();
        test_group.set_id("");
        api::pb::settings::TestGroupSettings &settings = *test_group.mutable_settings();
        api::pb::settings::ProcessSettings &process = *settings.mutable_process();
        {
            boost::optional<std::string> value;
            // resource limits
            api::pb::ResourceLimits &resource_limits = *process.mutable_resource_limits();
            if ((value = m_config.get_optional<std::string>("resource_limits.time")))
                resource_limits.set_time_limit_millis(detail::parse_time_millis(value.get()));
            if ((value = m_config.get_optional<std::string>("resource_limits.memory")))
                resource_limits.set_memory_limit_bytes(detail::parse_memory_bytes(value.get()));
            if ((value = m_config.get_optional<std::string>("resource_limits.output")))
                resource_limits.set_output_limit_bytes(detail::parse_memory_bytes(value.get()));
            // number of processes is not supported here
            if ((value = m_config.get_optional<std::string>("resource_limits.real_time")))
                resource_limits.set_real_time_limit_millis(detail::parse_time_millis(value.get()));
            // run
            api::pb::settings::Run &run = *settings.mutable_run();
            //run.set_order(); // depending on tests, is set in other location
            run.set_algorithm(api::pb::settings::Run::WHILE_NOT_FAIL);
            // files & execution
            api::pb::settings::File *file = process.add_files();
            api::pb::settings::Execution &execution = *process.mutable_execution();
            file->set_id("stdin");
            file->set_init("in");
            file->add_permissions(api::pb::settings::File::READ);
            if ((value = m_config.get_optional<std::string>("files.stdin")))
            {
                detail::to_pb_path(value.get(), *file->mutable_path());
            }
            else
            {
                api::pb::settings::Execution::Redirection &rd = *execution.add_redirections();
                rd.set_stream(api::pb::settings::Execution::Redirection::STDIN);
                rd.set_file_id("stdin");
            }
            file = process.add_files();
            file->set_id("stdout");
            file->add_permissions(api::pb::settings::File::READ);
            file->add_permissions(api::pb::settings::File::WRITE);
            if ((value = m_config.get_optional<std::string>("files.stdout")))
            {
                detail::to_pb_path(value.get(), *file->mutable_path());
            }
            else
            {
                api::pb::settings::Execution::Redirection &rd = *execution.add_redirections();
                rd.set_stream(api::pb::settings::Execution::Redirection::STDOUT);
                rd.set_file_id("stdout");
            }
        }
        test_group.clear_test_set();
        api::pb::testing::TestQuery &test_query = *test_group.add_test_set();
        test_query.mutable_wildcard()->set_value("*"); // select all tests
    }

    void simple0::read_utilities(api::pb::problem::Utilities &utilities)
    {
        const utility_ptr tests_ = tests(), checker_ = checker(), validator_ = validator();
        BOOST_ASSERT(tests_);
        *utilities.mutable_tests() = tests_->info();
        BOOST_ASSERT(checker_);
        *utilities.mutable_checker() = checker_->info();
        if (validator_)
            *utilities.mutable_validator() = validator_->info();
    }
}}}}
