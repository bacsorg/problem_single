#include "copy.hpp"

#include "bunsan/pm/index.hpp"

#include "bunsan/filesystem/operations.hpp"

#include <boost/property_tree/ini_parser.hpp>

namespace bacs{namespace single{namespace problem{namespace statement_versions
{
    const bool copy::factory_reg_hook = statement::version::register_new("copy",
        [](const boost::filesystem::path &location,
           const boost::property_tree::ptree &config)
        {
            statement::version_ptr tmp(new copy(location, config));
            return tmp;
        });

    copy::copy(const boost::filesystem::path &/*location*/,
               const boost::property_tree::ptree &config):
        m_lang(config.get<std::string>("info.lang")),
        m_source(config.get<std::string>("build.source")) {}


    void copy::make_package(const boost::filesystem::path &destination,
                            const bunsan::pm::entry &/*package*/,
                            const bunsan::pm::entry &resources_package) const
    {
        bunsan::filesystem::reset_dir(destination);
        bunsan::pm::index index;
        index.package.import.source.insert(std::make_pair("data", resources_package));
        index.package.self.insert(std::make_pair(".", "src"));
        boost::filesystem::create_directory(destination / "src");
        boost::property_tree::ptree statement_index;
        statement_index.put("data.index", m_source.string());
        boost::property_tree::write_ini((destination / "src" / "config.ini").string(), statement_index);
        index.save(destination / "index");
    }

    api::pb::problem::Statement::Version copy::info() const
    {
        api::pb::problem::Statement::Version v;
        {
            const std::string format = m_source.string();
            const std::size_t pos = format.rfind('.');
            if (pos == std::string::npos)
                BOOST_THROW_EXCEPTION(invalid_source_name_error() <<
                                      invalid_source_name_error::source_name(m_source));
            v.set_format(format.substr(pos + 1));
        }
        v.set_lang(m_lang);
        const bunsan::pm::entry package = bunsan::pm::entry(v.format()) / v.lang();
        v.set_package(package.name());
        return v;
    }
}}}}
