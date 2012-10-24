#include "single.hpp"

#include "bacs/single/problem/detail/split.hpp"

#include "bunsan/filesystem/operations.hpp"
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

    bool single::make_package(const boost::filesystem::path &destination,
                              const bunsan::pm::entry &/*package*/)
    {
        boost::filesystem::create_directories(destination);
        bunsan::pm::depends index;
        // builder itself
        index.source.import.source.insert(std::make_pair(".", "bacs/system/utility/single"));
        // sources, note: only build section is needed from config
        index.source.self.insert(std::make_pair("src", "src"));
        bunsan::filesystem::copy_tree(m_location, destination / "src");
        // dependencies
        for (const std::string &lib: m_libs)
            index.source.import.package.insert(std::make_pair(".", "bacs/lib/" + lib));
        // save it
        index.save(destination / "index");
        return true;
    }
}}}}
