#include "bunsan/config.hpp"

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
            config.put("call.wrapper", "test_id_iterate");
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
        index.source.import.package.insert(std::make_pair(".", "bacs/system/driver/simple0/tests"));
        index.source.self.insert(std::make_pair("share/tests", "tests"));
        boost::filesystem::create_directory(destination / "tests");
        {
            const boost::filesystem::directory_iterator end;
            for (boost::filesystem::directory_iterator i(m_location); i != end; ++i)
                boost::filesystem::copy_file(i->path(), destination / "tests" / i->path().filename());
        }
        index.save(destination / "index");
    }
}}}}
