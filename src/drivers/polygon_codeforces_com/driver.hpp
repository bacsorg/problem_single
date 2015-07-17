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

  Problem overview() const override;

  // utilities
  tests_ptr tests() const override;
  utility_ptr checker() const override;
  utility_ptr interactor() const override;

  statement_ptr statement() const override;

 private:
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
  single::tests::shared_ptr<polygon_codeforces_com::tests> m_tests;
  utility_ptr m_checker;
  statement_ptr m_statement;
};

}  // namespace polygon_codeforces_com
}  // namespace drivers
}  // namespace single
}  // namespace problem
}  // namespace bacs
