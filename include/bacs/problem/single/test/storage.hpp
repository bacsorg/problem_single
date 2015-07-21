#pragma once

#include <bacs/problem/single/problem.pb.h>
#include <bacs/problem/single/test/error.hpp>
#include <bacs/problem/utility.hpp>

#include <bunsan/factory_helper.hpp>

#include <unordered_set>

namespace bacs {
namespace problem {
namespace single {
namespace test {

class storage : public utility {
  BUNSAN_FACTORY_BODY(storage, const boost::filesystem::path & /*location*/,
                      const boost::property_tree::ptree & /*config*/)
 public:
  static storage_ptr instance(const boost::filesystem::path &location);

  using utility::utility;

 public:
  // create() will be provided by appropriate package
  virtual std::unordered_set<std::string> data_set() const = 0;
  virtual std::unordered_set<std::string> test_set() const = 0;

  Tests test_set_info() const;
};
BUNSAN_FACTORY_TYPES(storage)

}  // namespace test
}  // namespace single
}  // namespace problem
}  // namespace bacs
