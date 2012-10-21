#include "internal0.hpp"

#include "bunsan/pm/depends.hpp"

#include <boost/filesystem/operations.hpp>

namespace bacs{namespace single{namespace problem{namespace generators
{
    bool internal0::factory_reg_hook = generator::register_new("internal0",
        [](const boost::property_tree::ptree &config)
        {
            generator_ptr tmp(new internal0(config));
            return tmp;
        });

    internal0::internal0(const boost::property_tree::ptree &config) {}

    void internal0::generate(const options &options_)
    {
        boost::filesystem::remove_all(options_.destination);
        boost::filesystem::create_directories(options_.destination);
        // root package
        {
            const boost::filesystem::path package_root = options_.destination;
            const bunsan::pm::entry package = options_.root_package;
            boost::filesystem::create_directories(package_root);
            bunsan::pm::depends index;
            //index.source.import.source.insert(/*bacs::base*/); // source code  ??? Which
            //index.source.import.package.insert(/*bacs::base*/); // static libs ??? is better?
            //index.source.import.source.insert(/*checker*/); // function
            index.package.insert(std::make_pair(".", package / "tests")); // files
        }
        // checker package
        {
            const boost::filesystem::path package_root = options_.destination / "checker";
            const bunsan::pm::entry package = options_.root_package / "checker";
            boost::filesystem::create_directories(package_root);
            const utility_ptr checker = options_.driver->checker();
            BOOST_ASSERT(checker);
            checker->make_package(package_root, package);
            // TODO calling conventions
        }
        // validator package
        {
            const boost::filesystem::path package_root = options_.destination / "validator";
            const bunsan::pm::entry package = options_.root_package / "validator";
            boost::filesystem::create_directories(package_root);
            const utility_ptr validator = options_.driver->validator();
            if (validator)
            {
                validator->make_package(package_root, package);
                // TODO calling conventions
            }
            else
            {
                bunsan::pm::depends index;
                index.package.insert(std::make_pair(".", "bacs/system/validator/std/ok"));
                index.save(package_root / "index");
            }
        }
        // tests package
        {
            const boost::filesystem::path package_root = options_.destination / "tests";
            const bunsan::pm::entry package = options_.root_package / "tests";
            boost::filesystem::create_directories(package_root);
            bunsan::pm::depends index;
            // TODO use generator here
            index.save(package_root / "index");
        }
        // statement package
        {
            const boost::filesystem::path package_root = options_.destination / "statement";
            const bunsan::pm::entry package = options_.root_package / "statement";
            boost::filesystem::create_directories(package_root);
            bunsan::pm::depends index;
            index.save(package_root / "index");
        }
    }
}}}}
