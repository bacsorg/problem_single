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
        m_location(location),
        m_source(config.get<std::string>("source")),
        m_std(config.get_optional<std::string>("std")),
        m_libs(detail::get_vector(config, "libs")) {}

    api::pb::problem::Utility single::info() const
    {
        api::pb::problem::Utility info_;
        info_.set_builder("single");
        return info_;
    }

    void single::make_package(const boost::filesystem::path &destination,
                              const bunsan::pm::entry &package)
    {
        // TODO
    }
}}}}
