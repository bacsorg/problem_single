#include "single.hpp"

#include "bacs/single/problem/detail/split.hpp"

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
                              const bunsan::pm::entry &package)
    {
        // TODO
    }
}}}}
