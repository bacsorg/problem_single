#include <bacs/problem/single/importer.hpp>

#include <bacs/problem/single/driver.hpp>

#include <bunsan/static_initializer.hpp>

namespace bacs {
namespace problem {
namespace single {

static const char problem_type[] = "bacs/problem/single";

BUNSAN_STATIC_INITIALIZER(bacs_problem_single_importer, {
  BUNSAN_FACTORY_REGISTER(importer, problem::importer, problem_type,
                          [](const boost::property_tree::ptree &config) {
                            return problem::importer::make_shared<importer>(
                                config);
                          })
})

importer::importer(const boost::property_tree::ptree &config)
    : m_generator(generator::instance(
          config.get<std::string>("generator.type"),
          config.get_child("generator.config",
                           boost::property_tree::ptree()))) {}

Problem importer::convert(const options &options_) {
  try {
    generator::options goptions = {
        .driver = driver::instance(options_.problem_dir),
        .destination = options_.destination,
        .root_package = options_.root_package,
        .system = System(),
    };
    goptions.system.set_problem_type(problem_type);
    goptions.system.set_version(options_.version.data(),
                                options_.version.size());
    goptions.system.set_package(options_.root_package.name());
    return m_generator->generate(goptions);
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(importer_convert_error()
                          << importer_convert_error::options(options_)
                          << bunsan::enable_nested_current());
  }
}

}  // namespace single
}  // namespace problem
}  // namespace bacs
