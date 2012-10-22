#include "single.hpp"

#include "bacs/single/problem/detail/split.hpp"

#include "bunsan/pm/depends.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace bacs{namespace single{namespace problem{namespace utilities
{
    bool single::factory_reg_hook = utility::register_new("single",
        [](const boost::filesystem::path &location,
           const boost::property_tree::ptree &config)
        {
            utility_ptr tmp(new single(location, config));
            return tmp;
        });

    single::single(const boost::filesystem::path &location,
                   const boost::property_tree::ptree &config):
        utility(config),
        m_location(location),
        m_source(config.get<std::string>("build.source")),
        m_std(config.get_optional<std::string>("build.std")),
        m_libs(detail::get_vector(config, "build.libs")) {}

    void single::make_package(const boost::filesystem::path &destination,
                              const bunsan::pm::entry &/*package*/)
    {
        boost::filesystem::create_directories(destination);
        bunsan::pm::depends index;
        // sources
        {
            index.source.self.insert(std::make_pair(".", "src"));
            boost::property_tree::ptree config;
            config.put_child("build", section("build"));
            boost::filesystem::create_directories(destination / "src");
            boost::property_tree::write_ini((destination / "src" / "config.ini").string(), config);
            // TODO
        }
        // dependencies
        {
            // TODO libs
        }
        index.save(destination / "index");
    }
}}}}
