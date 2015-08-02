#pragma once

#include <bacs/problem/single/driver.hpp>
#include <bacs/problem/single/error.hpp>

#include <bunsan/factory_helper.hpp>
#include <bunsan/pm/entry.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/property_tree/ptree.hpp>

namespace bacs {
namespace problem {
namespace single {

namespace generator_detail {
struct options {
  driver_ptr driver;

  /// Root package directory
  boost::filesystem::path destination;

  bunsan::pm::entry root_package;

  System system;
};
}  // namespace generator_detail

struct generator_error : virtual error {};
struct generator_generate_error : virtual generator_error {
  using options =
      boost::error_info<struct tag_options, generator_detail::options>;
};

class generator : private boost::noncopyable {
  BUNSAN_FACTORY_BODY(generator, const boost::property_tree::ptree & /*config*/)
 public:
  using options = generator_detail::options;

 public:
  virtual Problem generate(const options &options_) = 0;
};
BUNSAN_FACTORY_TYPES(generator)

}  // namespace single
}  // namespace problem
}  // namespace bacs

namespace boost {
std::string to_string(
    const bacs::problem::single::generator_generate_error::options &options);
}  // namespace boost
