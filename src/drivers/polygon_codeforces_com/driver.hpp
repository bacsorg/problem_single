#pragma once

#include "tests.hpp"

#include "bacs/problem/single/driver.hpp"
#include "bacs/problem/single/error.hpp"

#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

namespace bacs {
namespace problem {
namespace single {
namespace drivers {
namespace polygon_codeforces_com {

class driver : public single::driver {
 public:
  explicit driver(const boost::filesystem::path &location);

  Problem overview() const override { return m_overview; }

  // utilities
  test::storage_ptr tests() const override { return m_tests; }
  utility_ptr checker() const override { return m_checker; }
  utility_ptr interactor() const override { return nullptr; }

  statement_ptr statement() const override { return m_statement; }

 private:
  void read_system();
  void read_info();
  void read_tests();
  void read_statement();
  void read_profiles();
  void read_checker();

 private:
  const boost::filesystem::path m_location;
  boost::property_tree::ptree m_config;
  boost::property_tree::ptree m_override_config;
  Problem m_overview;
  ProblemExtension m_overview_extension;
  single::test::storage::shared_ptr<polygon_codeforces_com::tests> m_tests;
  utility_ptr m_checker;
  statement_ptr m_statement;
};

}  // namespace polygon_codeforces_com
}  // namespace drivers
}  // namespace single
}  // namespace problem
}  // namespace bacs
