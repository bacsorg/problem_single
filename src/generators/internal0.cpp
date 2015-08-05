#include "internal0.hpp"

#include <bacs/problem/system_verifier.hpp>

#include <bunsan/filesystem/operations.hpp>
#include <bunsan/pm/index.hpp>
#include <bunsan/static_initializer.hpp>

namespace bacs {
namespace problem {
namespace single {
namespace generators {

BUNSAN_STATIC_INITIALIZER(bacs_problem_single_generators_internal0, {
  BUNSAN_FACTORY_REGISTER_TOKEN(
      generator, internal0,
      [](const boost::property_tree::ptree &config) {
        return generator::make_shared<internal0>(config);
      })
})

internal0::internal0(const boost::property_tree::ptree & /*config*/) {}

namespace {
void generate_verifier(const generator::options &options,
                       bunsan::pm::index &root_index) {
  const char name[] = "system_verifier";
  const boost::filesystem::path package_root = options.destination / name;
  const bunsan::pm::entry package = options.root_package / name;
  system_verifier verifier(options.system);
  if (verifier.make_package(package_root, package, options.system.revision()))
    root_index.package.import.package.insert(std::make_pair(".", package));
}

struct generator_generate_utility_error : virtual generator_generate_error {
  using utility_name = boost::error_info<struct tag_utility_name, std::string>;
};

struct generator_generate_utility_null_fallback_error
    : virtual generator_generate_utility_error {};

void generate_utility(const std::string &name, const std::string &internal_name,
                      const utility_ptr &utility_,
                      const generator::options &options,
                      bunsan::pm::index &root_index,
                      const char *const fallback = nullptr) {
  try {
    const boost::filesystem::path package_root = options.destination / name;
    const bunsan::pm::entry package = options.root_package / name;
    if (utility_) {
      if (utility_->make_package(package_root, package,
                                 options.system.revision())) {
        root_index.package.import.package.insert(std::make_pair(".", package));
      }
      // calling conventions
      root_index.package.import.package.insert(std::make_pair(
          ".", bunsan::pm::entry("bacs/system/single") / internal_name /
                   "call" /
                   utility_->section("utility").get<std::string>("call")));
      root_index.package.import.package.insert(std::make_pair(
          ".",
          bunsan::pm::entry("bacs/system/single") / internal_name / "return" /
              utility_->section("utility").get<std::string>("return", "none")));
    } else {
      if (!fallback)
        BOOST_THROW_EXCEPTION(generator_generate_utility_null_fallback_error());
      root_index.package.import.package.insert(
          std::make_pair(".", bunsan::pm::entry(fallback)));
      root_index.package.import.package.insert(
          std::make_pair(".", bunsan::pm::entry("bacs/system/single") /
                                  internal_name / "return" / "none"));
    }
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(generator_generate_utility_error()
                          << generator_generate_utility_error::utility_name(
                                 name) << bunsan::enable_nested_current());
  }
}
}  // namespace

Problem internal0::generate(const options &options_) {
  try {
    Problem problem_info = options_.driver->overview();
    *problem_info.mutable_system() = options_.system;

    // initialize package names
    for (Statement::Version &v :
         *problem_info.mutable_statement()->mutable_version()) {
      const bunsan::pm::entry package = options_.root_package / v.package();
      v.set_package(package.name());
    }

    // prepare sources
    bunsan::filesystem::reset_dir(options_.destination);
    bunsan::pm::index root_index;

    // root package
    root_index.package.import.package.insert(
        std::make_pair(".", "bacs/system/single"));

    BOOST_ASSERT(options_.driver->tests());
    if (!options_.driver->checker() && !options_.driver->interactor())
      BOOST_THROW_EXCEPTION(utility_error() << utility_error::message(
                                "Checker or interactor is required"));

    // system_verifier package
    generate_verifier(options_, root_index);

    // tests package
    generate_utility("tests", "tests", options_.driver->tests(), options_,
                     root_index);

    // checker package
    generate_utility("checker", "checker", options_.driver->checker(), options_,
                     root_index, "bacs/system/single/checker/call/std/ok");

    // interactor package
    generate_utility("interactor", "tester", options_.driver->interactor(),
                     options_, root_index,
                     "bacs/system/single/tester/call/std/standalone");

    // statement package
    {
      const boost::filesystem::path package_root =
          options_.destination / "statement";
      const bunsan::pm::entry package = options_.root_package / "statement";
      // statement is not included into problem's main package
      options_.driver->statement()->make_package(package_root, package,
                                                 options_.system.revision());
    }

    // the last command
    root_index.save(options_.destination / "index");
    return problem_info;
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(generator_generate_error()
                          << generator_generate_error::options(options_)
                          << bunsan::enable_nested_current());
  }
}

}  // namespace generators
}  // namespace single
}  // namespace problem
}  // namespace bacs
