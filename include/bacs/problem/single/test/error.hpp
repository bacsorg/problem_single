#pragma once

#include <bacs/problem/single/error.hpp>
#include <bacs/problem/utility.hpp>

namespace bacs {
namespace problem {
namespace single {
namespace test {

struct error : virtual single::error {
  using test_id = boost::error_info<struct tag_test_id, std::string>;
  using data_id = boost::error_info<struct tag_data_id, std::string>;
};
struct invalid_test_id_error : virtual invalid_id_error, virtual error {};
struct invalid_data_id_error : virtual invalid_id_error, virtual error {};
struct format_error : virtual error {};
struct data_format_error : virtual format_error {};
struct make_package_error : virtual utility_make_package_error,
                            virtual error {};

}  // namespace test
}  // namespace single
}  // namespace problem
}  // namespace bacs
