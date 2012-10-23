#include "bacs/single/problem/tests.hpp"

#include <boost/property_tree/ini_parser.hpp>

namespace bacs{namespace single{namespace problem
{
    BUNSAN_FACTORY_DEFINE(tests)

    tests_ptr tests::instance(const boost::filesystem::path &location)
    {
        boost::property_tree::ptree config;
        boost::property_tree::read_ini((location / "config.ini").string(), config);
        return instance(config.get<std::string>("build.builder"), location, config);
    }
}}}
