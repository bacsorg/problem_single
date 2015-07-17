#pragma once

#include <bacs/problem/error.hpp>
#include <bacs/problem/id.hpp>

namespace bacs {
namespace problem {
namespace single {

struct error : virtual problem::error {};
struct invalid_id_error : virtual error, virtual problem::invalid_id_error {};

struct driver_error : virtual error {};
struct buildable_error : virtual driver_error {};
struct utility_error : virtual buildable_error {};
struct statement_error : virtual buildable_error {};
struct checker_error : virtual utility_error {};
struct interactor_error : virtual utility_error {};

}  // namespace single
}  // namespace problem
}  // namespace bacs
