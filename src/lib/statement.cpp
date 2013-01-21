#include "bacs/single/problem/statement.hpp"

#include "bunsan/filesystem/operations.hpp"

#include "bunsan/pm/index.hpp"

#include <boost/property_tree/ini_parser.hpp>

namespace bacs{namespace single{namespace problem
{
    BUNSAN_FACTORY_DEFINE(statement::version)

    statement::version_ptr statement::version::instance(const boost::filesystem::path &config_location)
    {
        boost::property_tree::ptree config;
        boost::property_tree::read_ini(config_location.string(), config);
        return instance(config.get<std::string>("build.builder"), config_location.parent_path(), config);
    }

    statement::version::~version() {}

    namespace
    {
        const bunsan::pm::entry versions_subpackage = "versions";
    }

    statement_ptr statement::instance(const boost::filesystem::path &location)
    {
        const statement_ptr tmp(new statement(location));
        for (boost::filesystem::directory_iterator i(location), end; i != end; ++i)
        {
            if (i->path().filename().extension() == ".ini" &&
                boost::filesystem::is_regular_file(i->path()))
            {
                tmp->m_versions.push_back(version::instance(i->path()));
                api::pb::problem::Statement::Version &info = *(tmp->m_info.add_versions()) = tmp->m_versions.back()->info();
                const bunsan::pm::entry package = versions_subpackage / info.package();
                info.set_package(package.name());
            }
        }
        return tmp;
    }

    statement::statement(const boost::filesystem::path &location): m_location(location) {}

    bool statement::make_package(const boost::filesystem::path &destination,
                                 const bunsan::pm::entry &package) const
    {
        const bunsan::pm::entry &root_package = package;
        bunsan::filesystem::reset_dir(destination);
        // resources
        const bunsan::pm::entry resources_package = root_package / "resources";
        {
            const boost::filesystem::path package_root = destination / resources_package.location().filename();
            boost::filesystem::create_directory(package_root);
            bunsan::pm::index index;
            index.source.self.insert(std::make_pair("src", "src"));
            bunsan::filesystem::copy_tree(m_location, package_root / "src");
            index.save(package_root / "index");
        }
        // versions
        {
            const boost::filesystem::path versions_path = destination / versions_subpackage.location();
            boost::filesystem::create_directory(versions_path);
            for (const version_ptr &v: m_versions)
            {
                const bunsan::pm::entry package = v->info().package();
                v->make_package(versions_path / package.location(),
                                root_package / versions_subpackage / package,
                                resources_package);
            }
        }
        return true;
    }

    const api::pb::problem::Statement &statement::info() const
    {
        return m_info;
    }
}}}
