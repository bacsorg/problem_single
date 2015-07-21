#pragma once

#include <bacs/problem/single/test/error.hpp>
#include <bacs/problem/single/test/storage.hpp>

#include <bunsan/stream_enum.hpp>

#include <unordered_map>

namespace bacs {
namespace problem {
namespace single {
namespace test {

struct inconsistent_test_data_error : virtual error {};
struct inconsistent_test_data_set_error : virtual inconsistent_test_data_error {
};
struct empty_set_error : virtual error {};
struct unknown_data_error : virtual error {};

class list_storage : public storage {
 public:
  using test_data = std::unordered_map<std::string, boost::filesystem::path>;

  BUNSAN_INCLASS_STREAM_ENUM_CLASS(test_data_type, (text, binary))

 public:
  list_storage(const boost::filesystem::path &location,
               test_data_type default_data_type);

  void add_test(const std::string &test_id, const test_data &data);

  test_data_type data_type(const std::string &data_id) const;
  void set_data_type(const std::string &data_id, test_data_type type);

  std::unordered_set<std::string> data_set() const override;
  std::unordered_set<std::string> test_set() const override;

  bool make_package(const boost::filesystem::path &destination,
                    const bunsan::pm::entry &package) const override;

 protected:
  list_storage(const boost::filesystem::path &location,
               test_data_type default_data_type,
               const std::string &builder_name);

 private:
  const test_data_type m_default_data_type;
  std::unordered_map<std::string, test_data> m_tests;
  std::unordered_set<std::string> m_text_data_set;
};

}  // namespace test
}  // namespace single
}  // namespace problem
}  // namespace bacs
