#pragma once

#include <bacs/problem/single/test.pb.h>
#include <bacs/problem/single/test/error.hpp>

#include <memory>

namespace bacs {
namespace problem {
namespace single {
namespace test {

struct matcher_error : virtual error {};
struct matcher_not_set_error : virtual matcher_error {};

class matcher {
 public:
  explicit matcher(const Query &query);
  ~matcher();

  bool operator()(const std::string &test_id) const;

 private:
  class impl;
  class id;
  class wildcard;
  class regex;

  std::unique_ptr<impl> make_query(const Query &query);

 private:
  std::unique_ptr<impl> m_impl;
};

}  // namespace test
}  // namespace single
}  // namespace problem
}  // namespace bacs
