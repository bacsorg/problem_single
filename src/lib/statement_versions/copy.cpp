#include "copy.hpp"

#include "bunsan/pm/index.hpp"

#include "bunsan/config/cast.hpp"
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

    namespace
    {
        std::string get_format(const boost::filesystem::path &source)
        {
            const std::string filename = source.string();
            const std::size_t pos = filename.rfind('.');
            if (pos == std::string::npos)
                BOOST_THROW_EXCEPTION(invalid_source_name_error() <<
                                      invalid_source_name_error::source_name(source));
            return filename.substr(pos + 1);
        }
    }

    copy::copy(const boost::filesystem::path &/*location*/,
               const boost::property_tree::ptree &config):
        version(config.get<std::string>("info.lang"), get_format(config.get<std::string>("build.source"))),
        m_source(config.get<std::string>("build.source")) {}

    void copy::make_package(const boost::filesystem::path &destination,
                            const bunsan::pm::entry &/*package*/,
                            const bunsan::pm::entry &resources_package) const
    {
        bunsan::filesystem::reset_dir(destination);
        bunsan::pm::index index;
        index.package.import.source.insert(std::make_pair("data", resources_package));
        index.package.self.insert(std::make_pair(".", "pkg"));
        boost::filesystem::create_directory(destination / "pkg");
        manifest statement_manifest;
        statement_manifest.version.lang = lang();
        statement_manifest.version.format = format();
        statement_manifest.data.index = m_source;
        boost::property_tree::write_ini((destination / "pkg" / "manifest.ini").string(),
                                        bunsan::config::save<boost::property_tree::ptree>(statement_manifest));
        index.save(destination / "index");
    }

    api::pb::problem::Statement::Version copy::info() const
    {
        api::pb::problem::Statement::Version v;
        v.set_lang(lang());
        v.set_format(format());
        v.set_package(subpackage().name());
        return v;
    }
}}}}
