#pragma once

#include <bacs/problem/single/error.hpp>

#include <bacs/problem/statement.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>

namespace bacs {
namespace problem {
namespace single {
namespace drivers {
namespace polygon_codeforces_com {

struct invalid_statement_version_path_error : virtual statement_error {};

class statement : public problem::statement {
 public:
  statement(const boost::filesystem::path &location,
            const boost::property_tree::ptree &config);

 private:
  class version : public problem::statement::version {
   public:
    version(const std::string &language, const std::string &format,
            const boost::filesystem::path &source);

    void make_package(const boost::filesystem::path &destination,
                      const bunsan::pm::entry &package,
                      const bunsan::pm::entry &resources_package,
                      const Revision &revision) const override;

   private:
    const boost::filesystem::path m_root, m_source;
  };

  std::vector<version_ptr> parse_versions(
      const std::string &prefix, const boost::property_tree::ptree &config);
};

}  // namespace polygon_codeforces_com
}  // namespace drivers
}  // namespace single
}  // namespace problem
}  // namespace bacs
