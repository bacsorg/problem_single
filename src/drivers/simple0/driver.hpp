#pragma once

#include <bacs/problem/single/driver.hpp>
#include <bacs/problem/single/error.hpp>
#include <bacs/problem/single/process/settings.pb.h>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

#include <map>
#include <string>
#include <vector>

namespace bacs {
namespace problem {
namespace single {
namespace drivers {
namespace simple0 {

struct invalid_group_id_error : virtual error {};
struct empty_group_id_error : virtual invalid_group_id_error {};
struct invalid_continue_condition_error : virtual error {
  using continue_condition =
      boost::error_info<struct tag_continue_condition, std::string>;
};

/// \ref format_simple0_page implementation
class driver : public single::driver {
 public:
  explicit driver(const boost::filesystem::path &location);

  Problem overview() const override { return m_overview; }

  // utilities
  test::storage_ptr tests() const override { return m_tests; }
  utility_ptr checker() const override { return m_checker; }
  utility_ptr interactor() const override { return m_interactor; }

  statement_ptr statement() const override { return m_statement; }

 private:
  void read_system();
  void read_info();
  void read_tests();
  void read_statement();
  void read_settings();
  void read_profiles();
  void read_checker();
  void read_interactor();

 private:
  const boost::filesystem::path m_location;
  boost::property_tree::ptree m_config;
  process::Settings m_settings;
  Problem m_overview;
  ProblemExtension m_overview_extension;
  test::storage_ptr m_tests;
  utility_ptr m_checker;
  utility_ptr m_interactor;
  statement_ptr m_statement;

  // sorted
  std::map<std::string, TestGroup> m_test_groups;
};

}  // namespace simple0
}  // namespace drivers
}  // namespace single
}  // namespace problem
}  // namespace bacs
