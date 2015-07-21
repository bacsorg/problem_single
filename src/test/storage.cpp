#include <bacs/problem/single/test/storage.hpp>

#include <boost/property_tree/ini_parser.hpp>

namespace bacs {
namespace problem {
namespace single {
namespace test {

BUNSAN_FACTORY_DEFINE(storage)

storage_ptr storage::instance(const boost::filesystem::path &location) {
  boost::property_tree::ptree config;
  boost::property_tree::read_ini((location / "config.ini").string(), config);
  return instance(config.get<std::string>("build.builder"), location, config);
}

Tests storage::test_set_info() const {
  Tests info;
  for (const std::string &data_id : data_set()) info.add_data_set(data_id);
  for (const std::string &test_id : test_set()) info.add_test_set(test_id);
  return info;
}

}  // namespace test
}  // namespace single
}  // namespace problem
}  // namespace bacs
