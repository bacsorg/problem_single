#include <bunsan/config.hpp>

#include <bacs/problem/single/test/list_storage.hpp>

#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/pm/index.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/iterator_range.hpp>
#include <bunsan/serialization/unordered_set.hpp>

namespace bacs {
namespace problem {
namespace single {
namespace test {

namespace {
boost::property_tree::ptree get_config(const std::string &builder_name) {
  boost::property_tree::ptree config;
  config.put("build.builder", builder_name);
  config.put("utility.call", "copy");
  return config;
}

template <typename T>
std::unordered_set<std::string> map_keys(
    const std::unordered_map<std::string, T> &map) {
  return boost::copy_range<std::unordered_set<std::string>>(
      map | boost::adaptors::map_keys);
}
}  // namespace

list_storage::list_storage(const boost::filesystem::path &location,
                           const test_data_type default_data_type,
                           const std::string &builder_name)
    : storage(location, get_config(builder_name)),
      m_default_data_type(default_data_type) {}

list_storage::list_storage(const boost::filesystem::path &location,
                           const test_data_type default_data_type)
    : list_storage(location, default_data_type,
                   "[list_storage tests generator]") {}

void list_storage::add_test(const std::string &test_id, const test_data &data) {
  if (m_tests.empty()) {
    m_tests[test_id] = data;
    for (const auto &id_file : data)
      set_data_type(id_file.first, m_default_data_type);
  } else {
    const auto &test = *m_tests.begin();
    if (map_keys(data) != map_keys(test.second))
      BOOST_THROW_EXCEPTION(inconsistent_test_data_set_error());
    const auto iter = m_tests.find(test_id);
    if (iter == m_tests.end()) {
      m_tests[test_id] = data;
    } else {
      if (iter->second != data)
        BOOST_THROW_EXCEPTION(inconsistent_test_data_error()
                              << inconsistent_test_data_error::message(
                                  "Tests with the same id "
                                  "have different data files."));
    }
  }
}

list_storage::test_data_type list_storage::data_type(
    const std::string &data_id) const {
  return m_text_data_set.find(data_id) == m_text_data_set.end()
             ? test_data_type::binary
             : test_data_type::text;
}

void list_storage::set_data_type(const std::string &data_id,
                                 const test_data_type type) {
  if (m_tests.empty()) BOOST_THROW_EXCEPTION(empty_set_error());
  if (m_tests.begin()->second.find(data_id) == m_tests.begin()->second.end())
    BOOST_THROW_EXCEPTION(unknown_data_error()
                          << unknown_data_error::data_id(data_id));
  switch (type) {
    case test_data_type::binary:
      m_text_data_set.erase(data_id);
      break;
    case test_data_type::text:
      m_text_data_set.insert(data_id);
      break;
    default:
      BOOST_ASSERT(false);
  }
}

std::unordered_set<std::string> list_storage::data_set() const {
  if (m_tests.empty()) BOOST_THROW_EXCEPTION(empty_set_error());
  return map_keys(m_tests.begin()->second);
}

std::unordered_set<std::string> list_storage::test_set() const {
  return map_keys(m_tests);
}

bool list_storage::make_package(const boost::filesystem::path &destination,
                                const bunsan::pm::entry & /*package*/) const {
  try {
    boost::filesystem::create_directories(destination);
    bunsan::pm::index index;
    // tests builder
    index.source.import.package.insert(
        std::make_pair(".", "bacs/system/single/list_tests"));
    // tests
    index.source.self.insert(std::make_pair("share/tests", "tests"));
    boost::filesystem::create_directory(destination / "tests");
    for (const auto &id_data : m_tests) {
      for (const auto &id_path : id_data.second)
        boost::filesystem::copy_file(
            location() / id_path.second,
            destination / "tests" / (id_data.first + "." + id_path.first));
    }
    // configuration for tests generator
    index.source.self.insert(std::make_pair("etc", "etc"));
    boost::filesystem::create_directory(destination / "etc");
    bunsan::filesystem::ofstream fout(destination / "etc/tests");
    BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fout) {
      // note: test_set_ and data_set_ are not altered,
      // but boost::oarchive interface does not support
      // const references in operator<<()
      const std::unordered_set<std::string> test_set_ = test_set();
      const std::unordered_set<std::string> data_set_ = data_set();
      boost::archive::text_oarchive oa(fout);
      // FIXME order is hard-coded
      // Should be moved into separate header.
      oa << test_set_ << data_set_ << m_text_data_set;
    } BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
    fout.close();
    index.save(destination / "index");
    return true;
  } catch (std::exception &) {
    BOOST_THROW_EXCEPTION(make_package_error()
                          << make_package_error::destination(destination)
                          // << make_package_error::package(package)
                          << bunsan::enable_nested_current());
  }
}

}  // namespace test
}  // namespace single
}  // namespace problem
}  // namespace bacs
