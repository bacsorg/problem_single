#include "single.hpp"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace bacs{namespace single{namespace problem{namespace utilities
{
    bool single::factory_reg_hook = utility::register_new("single",
        [](const boost::filesystem::path &location,
           const boost::property_tree::ptree &config)
        {
            utility_ptr tmp(new single(location, config));
            return tmp;
        });

    namespace
    {
        template <typename Ptree>
        std::vector<std::string> get_vector(
            const Ptree &config, const typename Ptree::path_type &name)
        {
            const boost::optional<std::string> val =
                config.template get_optional<std::string>(name);
            std::vector<std::string> vec;
            if (val)
                boost::algorithm::split(
                    vec, val.get(), boost::algorithm::is_space(),
                    boost::algorithm::token_compress_on);
            return vec;
        }
    }

    single::single(const boost::filesystem::path &location,
                   const boost::property_tree::ptree &config):
        m_location(location),
        m_source(config.get<std::string>("source")),
        m_std(config.get_optional<std::string>("std")),
        m_libs(get_vector(config, "libs")) {}

    void single::make_package(const boost::filesystem::path &destination)
    {
        // TODO
    }
}}}}
