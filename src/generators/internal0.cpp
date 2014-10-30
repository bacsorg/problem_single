#include "internal0.hpp"

#include <bunsan/filesystem/operations.hpp>
#include <bunsan/pm/index.hpp>

namespace bacs{namespace problem{namespace single{namespace generators
{
    const bool internal0::factory_reg_hook = generator::register_new("internal0",
        [](const boost::property_tree::ptree &config)
        {
            generator_ptr tmp(new internal0(config));
            return tmp;
        });

    internal0::internal0(const boost::property_tree::ptree &/*config*/) {}

    namespace
    {
        void generate_utility(
            const std::string &name,
            const utility_ptr &utility_,
            const generator::options &options,
            bunsan::pm::index &root_index,
            const char *const fallback=nullptr)
        {
            const boost::filesystem::path package_root =
                options.destination / name;
            const bunsan::pm::entry package =
                options.root_package / name;
            if (utility_)
            {
                if (utility_->make_package(package_root, package))
                    root_index.package.import.package.insert(
                        std::make_pair(".", package)
                    );
                // calling conventions
                root_index.source.import.source.insert(std::make_pair(".",
                    bunsan::pm::entry("bacs/system/single") / name / "call" /
                    utility_->section("utility").get<std::string>("call")));
            }
            else
            {
                BOOST_ASSERT_MSG(!fallback, name.c_str());
                root_index.source.import.source.insert(std::make_pair(
                    ".",
                    bunsan::pm::entry(fallback)
                ));
            }
        }
    }

    Problem internal0::generate(const options &options_)
    {
        try
        {
            Problem problem_info = options_.driver->overview();

            // initialize package names
            problem_info.mutable_info()->mutable_system()->set_package(
                options_.root_package.name());
            for (Statement::Version &v:
                    *problem_info.mutable_statement()->mutable_version()
                )
            {
                const bunsan::pm::entry package =
                    options_.root_package / v.package();
                v.set_package(package.name());
            }

            // prepare sources
            bunsan::filesystem::reset_dir(options_.destination);
            bunsan::pm::index root_index;

            // root package
            {
                // \note: package directory is already created,
                // package does not contain sources
                // TODO: think about library import
                root_index.source.import.source.insert(
                    std::make_pair(".", "bacs/system/single"));
            }

            BOOST_ASSERT(options_.driver->tests());
            if (!options_.driver->checker() && !options_.driver->interactor())
                BOOST_THROW_EXCEPTION(
                    utility_error() <<
                    utility_error::message("Checker or interactor is required"));

            // tests package
            generate_utility(
                "tests",
                options_.driver->tests(),
                options_,
                root_index
            );

            // checker package
            generate_utility(
                "checker",
                options_.driver->checker(),
                options_,
                root_index,
                "bacs/system/single/checker/call/std/ok"
            );

            // interactor package
            generate_utility(
                "interactor",
                options_.driver->interactor(),
                options_,
                root_index,
                "bacs/system/single/tester/call/std/standalone"
            );

            // statement package
            {
                const boost::filesystem::path package_root =
                    options_.destination / "statement";
                const bunsan::pm::entry package =
                    options_.root_package / "statement";
                (void) options_.driver->statement()->make_package(
                    package_root,
                    package
                );
            }

            // the last command
            root_index.save(options_.destination / "index");
            return problem_info;
        }
        catch (std::exception &)
        {
            BOOST_THROW_EXCEPTION(
                generator_generate_error() <<
                generator_generate_error::options(options_) <<
                bunsan::enable_nested_current());
        }
    }
}}}}
