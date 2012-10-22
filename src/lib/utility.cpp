#include "bacs/single/problem/utility.hpp"

#include <boost/property_tree/ini_parser.hpp>

namespace bacs{namespace single{namespace problem
{
    BUNSAN_FACTORY_DEFINE(utility)

    utility_ptr utility::instance(const boost::filesystem::path &location)
    {
        boost::property_tree::ptree config;
        boost::property_tree::read_ini((location / "config.ini").string(), config);
        // FIXME Is it OK to hardcode "single" here?
        return instance(config.get<std::string>("builder", "single"), location, config);
    }

    utility::utility(const boost::property_tree::ptree &config):
        m_config(config) {}

    api::pb::problem::Utility utility::info() const
    {
        api::pb::problem::Utility info_;
        info_.set_builder(m_config.get<std::string>("build.builder"));
        return info_;
    }

    boost::property_tree::ptree utility::section(const std::string &name) const
    {
        return m_config.get_child(name);
    }
}}}
