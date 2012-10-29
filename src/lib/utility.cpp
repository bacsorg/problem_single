#include "bacs/single/problem/utility.hpp"
#include "bacs/single/problem/error.hpp"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem/operations.hpp>

namespace bacs{namespace single{namespace problem
{
    BUNSAN_FACTORY_DEFINE(utility)

    utility_ptr utility::instance(const boost::filesystem::path &location)
    {
        boost::property_tree::ptree config;
        boost::property_tree::read_ini((location / "config.ini").string(), config);
        // note: "no" is equivalent to empty section
        return instance(config.get<std::string>("build.builder", "no"), location, config);
    }

    utility::utility(const boost::filesystem::path &location,
                     const boost::property_tree::ptree &config):
        m_location(location),
        m_target(config.get<std::string>("build.target",
            boost::filesystem::absolute(location).filename().string())),
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

    boost::filesystem::path utility::location() const
    {
        return m_location;
    }

    boost::filesystem::path utility::target() const
    {
        if (m_target.filename() != m_target)
            BOOST_THROW_EXCEPTION(error() << error::message("Invalid target name."));
        return m_target;
    }
}}}
