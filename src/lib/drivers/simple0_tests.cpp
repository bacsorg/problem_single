#include "simple0_tests.hpp"

#include "bunsan/pm/depends.hpp"

#include <boost/filesystem/operations.hpp>

namespace bacs{namespace single{namespace problem{namespace drivers
{
    static const boost::property_tree::ptree config =
        []()
        {
            boost::property_tree::ptree config;
            config.put("build.builder", "[simple0 tests generator]");
            config.put("call.wrapper", "TODO");// TODO
            return config;
        }();

    simple0_tests::simple0_tests(const boost::filesystem::path &location):
        utility(config),
        m_location(location) {}

    void simple0_tests::make_package(const boost::filesystem::path &destination,
                                     const bunsan::pm::entry &package)
    {
        boost::filesystem::create_directories(destination);
        bunsan::pm::depends index;
        // TODO
        index.save(destination / "index");
    }
}}}}
