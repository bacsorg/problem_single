#include "internal0.hpp"

#include "bunsan/filesystem/operations.hpp"

#include "bunsan/pm/index.hpp"

namespace bacs{namespace problem{namespace single{namespace generators
{
    const bool internal0::factory_reg_hook = generator::register_new("internal0",
        [](const boost::property_tree::ptree &config)
        {
            generator_ptr tmp(new internal0(config));
            return tmp;
        });

    internal0::internal0(const boost::property_tree::ptree &/*config*/) {}

    pb::problem::Problem internal0::generate(const options &options_)
    {
        pb::problem::Problem problem_info = options_.driver->overview();
        // initialize package names
        problem_info.mutable_info()->mutable_system()->set_package(options_.root_package.name());
        for (pb::problem::Statement::Version &v: *problem_info.mutable_statement()->mutable_versions())
        {
            const bunsan::pm::entry package = options_.root_package / v.package();
            v.set_package(package.name());
        }
        // prepare sources
        bunsan::filesystem::reset_dir(options_.destination);
        bunsan::pm::index root_index;
        // root package
        {
            // \note: package directory is already created, package does not contain sources
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
                root_index.package.import.package.insert(std::make_pair(".", package));
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
                root_index.package.import.package.insert(std::make_pair(".", package));
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
            bunsan::pm::index index;
            if (validator)
            {
                if (validator->make_package(package_root, package))
                    root_index.package.import.package.insert(std::make_pair(".", package));
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
            (void) options_.driver->statement()->make_package(package_root, package);
        }
        // the last command
        root_index.save(options_.destination / "index");
        return problem_info;
    }
}}}}
