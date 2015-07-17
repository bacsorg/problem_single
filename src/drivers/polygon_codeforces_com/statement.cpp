#include "statement.hpp"

#include <bunsan/config/cast.hpp>
#include <bunsan/filesystem/operations.hpp>
#include <bunsan/pm/index.hpp>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace bacs {
namespace problem {
namespace single {
namespace drivers {
namespace polygon_codeforces_com {

statement::statement(const boost::filesystem::path &location,
                     const boost::property_tree::ptree &config)
    : problem::statement(
          location,
          parse_versions(location.filename().string() + "/", config)) {}

std::vector<statement::version_ptr> statement::parse_versions(
    const std::string &prefix, const boost::property_tree::ptree &config) {
  std::vector<version_ptr> versions;
  for (const boost::property_tree::ptree::value_type &statement_version :
       config) {
    if (statement_version.first == "<xmlattr>") continue;
    // format
    // TODO format -> mime_type
    std::string format = statement_version.second.get<std::string>(
        "<xmlattr>.type", "unknown/unknown");
    const std::size_t sep = format.rfind('/');
    if (sep != std::string::npos) format = format.substr(sep + 1);

    // path
    std::string path =
        statement_version.second.get<std::string>("<xmlattr>.path");
    if (boost::algorithm::starts_with(path, prefix)) {
      path = path.substr(prefix.size());
    } else {
      BOOST_THROW_EXCEPTION(
          invalid_statement_version_path_error()
          << invalid_statement_version_path_error::path(path));
    }

    // commit
    versions.push_back(statement::version::make_shared<version>(
        statement_version.second.get<std::string>("<xmlattr>.language", "C"),
        format, path));
  }
  return versions;
}

statement::version::version(const std::string &lang, const std::string &format,
                            const boost::filesystem::path &source)
    : problem::statement::version(lang, format), m_source(source) {}

void statement::version::make_package(
    const boost::filesystem::path &destination,
    const bunsan::pm::entry & /*package*/,
    const bunsan::pm::entry &resources_package) const {
  try {
    bunsan::filesystem::reset_dir(destination);
    bunsan::pm::index index;
    index.package.import.source.insert(
        std::make_pair("data", resources_package));
    index.package.self.insert(std::make_pair(".", "pkg"));
    boost::filesystem::create_directory(destination / "pkg");
    manifest statement_manifest;
    statement_manifest.version.lang = lang();
    statement_manifest.version.format = format();
    statement_manifest.data.index = m_source;
    boost::property_tree::write_ini(
        (destination / "pkg" / manifest_path).string(),
        bunsan::config::save<boost::property_tree::ptree>(statement_manifest));
    index.save(destination / "index");
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(
        statement_version_make_package_error()
        << statement_version_make_package_error::destination(destination) <<
        // statement_version_make_package_error::package(package) <<
        statement_version_make_package_error::resources_package(
            resources_package) << bunsan::enable_nested_current());
  }
}

}  // namespace polygon_codeforces_com
}  // namespace drivers
}  // namespace single
}  // namespace problem
}  // namespace bacs
