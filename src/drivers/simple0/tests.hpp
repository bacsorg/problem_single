#pragma once

#include <bacs/problem/single/test/error.hpp>
#include <bacs/problem/single/test/list_storage.hpp>

namespace bacs {
namespace problem {
namespace single {
namespace drivers {
namespace simple0 {

struct test_no_in_data_error : virtual test::data_format_error {};
struct test_unknown_data_error : virtual test::data_format_error {};

class tests : public test::storage {
 public:
  using test::storage::storage;

 public:
  using test::storage::instance;

  static storage_ptr instance(const boost::filesystem::path &location,
                              const boost::property_tree::ptree &config);

  explicit tests(const boost::filesystem::path &location,
                 const boost::property_tree::ptree &config);

  std::unordered_set<std::string> data_set() const override {
    return m_tests.data_set();
  }

  std::unordered_set<std::string> test_set() const override {
    return m_tests.test_set();
  }

  bool make_package(const boost::filesystem::path &destination,
                    const bunsan::pm::entry &package) const {
    return m_tests.make_package(destination, package);
  }

 private:
  test::list_storage m_tests;
};

}  // namespace simple0
}  // namespace drivers
}  // namespace single
}  // namespace problem
}  // namespace bacs
