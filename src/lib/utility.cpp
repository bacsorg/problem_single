#include "bacs/single/problem/utility.hpp"

#include <boost/property_tree/ini_parser.hpp>

namespace bacs{namespace single{namespace problem
{
    BUNSAN_FACTORY_DEFINE(utility)

    utility_ptr utility::instance(const boost::filesystem::path &location)
    {
        boost::property_tree::ptree config;
        boost::property_tree::read_ini((location / "config.ini").string(), config);
        return instance(config.get<std::string>("builder", "single"), location, config);
    }
}}}
