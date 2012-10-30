#include "internal0.hpp"

#include "bunsan/pm/depends.hpp"

#include <boost/filesystem/operations.hpp>

namespace bacs{namespace single{namespace problem{namespace generators
{
    const bool internal0::factory_reg_hook = generator::register_new("internal0",
        [](const boost::property_tree::ptree &config)
        {
            generator_ptr tmp(new internal0(config));
            return tmp;
        });

    internal0::internal0(const boost::property_tree::ptree &/*config*/) {}

    api::pb::problem::Problem internal0::generate(const options &options_)
    {
        api::pb::problem::Problem problem_info = options_.driver->overview();
        problem_info.mutable_info()->mutable_system()->set_package(options_.root_package.name());
        boost::filesystem::remove_all(options_.destination);
        boost::filesystem::create_directories(options_.destination);
        bunsan::pm::depends root_index;
        // root package
        {
            boost::filesystem::create_directory(options_.destination);
            // TODO: think about library import
            root_index.source.import.source.insert(std::make_pair(".", "bacs/system/single"));
        }
        // tests package
        {
            const boost::filesystem::path package_root = options_.destination / "tests";
            const bunsan::pm::entry package = options_.root_package / "tests";
            const utility_ptr tests = options_.driver->tests();
            BOOST_ASSERT(tests);
            if (tests->make_package(package_root, package))
                root_index.package.insert(std::make_pair(".", package));
            // calling conventions
            root_index.source.import.source.insert(std::make_pair(".",
                bunsan::pm::entry("bacs/system/tests/call") /
                tests->section("utility").get<std::string>("call")));
        }
        // checker package
        {
            const boost::filesystem::path package_root = options_.destination / "checker";
            const bunsan::pm::entry package = options_.root_package / "checker";
            const utility_ptr checker = options_.driver->checker();
            BOOST_ASSERT(checker);
            if (checker->make_package(package_root, package))
                root_index.package.insert(std::make_pair(".", package));
            // calling conventions
            root_index.source.import.source.insert(std::make_pair(".",
                bunsan::pm::entry("bacs/system/checker/call") /
                checker->section("utility").get<std::string>("call")));
        }
        // validator package
        {
            const boost::filesystem::path package_root = options_.destination / "validator";
            const bunsan::pm::entry package = options_.root_package / "validator";
            const utility_ptr validator = options_.driver->validator();
            bunsan::pm::depends index;
            if (validator)
            {
                if (validator->make_package(package_root, package))
                    root_index.package.insert(std::make_pair(".", package));
                // calling conventions
                root_index.source.import.source.insert(std::make_pair(".",
                    bunsan::pm::entry("bacs/system/validator/call") /
                    validator->section("utility").get<std::string>("call")));
            }
            else
            {
                root_index.source.import.source.insert(std::make_pair(".", "bacs/system/validator/std/ok"));
            }
        }
        // statement package
        {
            const boost::filesystem::path package_root = options_.destination / "statement";
            const bunsan::pm::entry package = options_.root_package / "statement";
            boost::filesystem::create_directory(package_root);
            bunsan::pm::depends index;
            index.save(package_root / "index");
        }
        // the last command
        root_index.save(options_.destination / "index");
        return problem_info;
    }
}}}}
