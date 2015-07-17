#pragma once

#include <bacs/problem/single/detail/list_tests.hpp>

namespace bacs {
namespace problem {
namespace single {
namespace drivers {
namespace polygon_codeforces_com {

class tests : public detail::list_tests {
 public:
  explicit tests(const boost::filesystem::path &location);

  void add_test(const std::string &test_id, const boost::filesystem::path &in,
                const boost::filesystem::path &out);
};

}  // namespace polygon_codeforces_com
}  // namespace drivers
}  // namespace single
}  // namespace problem
}  // namespace bacs
