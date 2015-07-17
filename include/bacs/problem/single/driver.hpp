#pragma once

#include <bacs/problem/single/tests.hpp>

#include <bacs/problem/problem.pb.h>
#include <bacs/problem/statement.hpp>
#include <bacs/problem/utility.hpp>

#include <bunsan/factory_helper.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>

namespace bacs {
namespace problem {
namespace single {

class driver : private boost::noncopyable {
  BUNSAN_FACTORY_BODY(driver, const boost::filesystem::path & /*location*/)
 public:
  static driver_ptr instance(const boost::filesystem::path &location);

 public:
  /*!
   * \warning package names are relative to problem root package,
   * i.e. they should be prepended with root entry for problem,
   * e.g. "tests" package should become "bacs/problem/1000/tests",
   * where "bacs/problem/1000" corresponds problem
   *
   * \warning package name itself should be initialized later
   *
   * \warning hash should be initialized later
   */
  virtual Problem overview() const = 0;

  // utilities
  virtual tests_ptr tests() const = 0;

  virtual utility_ptr checker() const = 0;

  virtual utility_ptr interactor() const = 0;

  virtual statement_ptr statement() const = 0;
};
BUNSAN_FACTORY_TYPES(driver)

}  // namespace single
}  // namespace problem
}  // namespace bacs
