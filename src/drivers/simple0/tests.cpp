#include "tests.hpp"

#include <bunsan/enable_error_info.hpp>
#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/pm/index.hpp>
#include <bunsan/static_initializer.hpp>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <bunsan/serialization/unordered_set.hpp>

#include <unordered_map>

namespace bacs {
namespace problem {
namespace single {
namespace drivers {
namespace simple0 {

BUNSAN_STATIC_INITIALIZER(bacs_problem_single_drivers_simple0_tests, {
  BUNSAN_FACTORY_REGISTER_TOKEN(tests, simple0_embedded,
                                [](const boost::filesystem::path &location,
                                   const boost::property_tree::ptree &config) {
                                  return test::storage::make_shared<tests>(
                                      location, config);
                                })
})

test::storage_ptr tests::instance(const boost::filesystem::path &location,
                                  const boost::property_tree::ptree &config) {
  boost::property_tree::ptree cfg = config;
  const std::string type = config.get<std::string>("type", "simple0_embedded");
  if (type != "simple0_embedded") {
    boost::property_tree::read_info((location / "config.ini").string(), cfg);
  }
  return instance(type, location, cfg);
}

static boost::property_tree::ptree get_config(
    const boost::property_tree::ptree &config) {
  boost::property_tree::ptree cfg;
  cfg.put("build.builder", "simple0_embedded");
  cfg.put("utility.call", "copy");
  cfg.put_child("info", config);
  return cfg;
}

static const std::string data_prefix = "data_";

tests::tests(const boost::filesystem::path &location,
             const boost::property_tree::ptree &config)
    : test::storage(location, get_config(config)),
      m_tests(location, test::list_storage::test_data_type::text) {
  using test_data = test::list_storage::test_data;
  using test_data_type = test::list_storage::test_data_type;

  std::unordered_map<std::string, std::unordered_set<std::string>> test_files;
  for (boost::filesystem::directory_iterator i(location), end; i != end; ++i) {
    if (!boost::filesystem::is_regular_file(i->path()))
      BOOST_THROW_EXCEPTION(test::format_error());
    const std::string fname = i->path().filename().string();
    const std::string::size_type dot = fname.find('.');
    if (dot == std::string::npos) BOOST_THROW_EXCEPTION(test::format_error());
    if (fname.find('.', dot + 1) != std::string::npos)
      BOOST_THROW_EXCEPTION(test::format_error());
    const std::string test_id = fname.substr(0, dot);
    const std::string data_id = fname.substr(dot + 1);
    test_files[test_id].insert(data_id);
  }
  for (const auto &id_data : test_files) {
    const std::string &test_id = id_data.first;
    test_data data;
    for (const auto &data_id : id_data.second)
      data[data_id] = test_id + "." + data_id;
    m_tests.add_test(test_id, data);
  }

  // override data types from config
  for (const auto kv : config) {
    const std::string &key = kv.first;
    if (boost::algorithm::starts_with(key, data_prefix)) {
      const std::string data_id = key.substr(data_prefix.size());
      const auto data_type = boost::lexical_cast<test_data_type>(
          kv.second.get_value<std::string>());
      m_tests.set_data_type(data_id, data_type);
    }
  }
}

}  // namespace simple0
}  // namespace drivers
}  // namespace single
}  // namespace problem
}  // namespace bacs
