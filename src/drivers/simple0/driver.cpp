#include "driver.hpp"

#include "tests.hpp"

#include <bacs/file.hpp>
#include <bacs/problem/single/error.hpp>
#include <bacs/problem/single/problem.pb.h>

#include <bacs/problem/split.hpp>
#include <bacs/problem/resource/parse.hpp>

#include <bunsan/static_initializer.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/assert.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/range/iterator_range.hpp>

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

namespace bacs {
namespace problem {
namespace single {
namespace drivers {
namespace simple0 {

BUNSAN_STATIC_INITIALIZER(bacs_problem_single_drivers_simple0, {
  BUNSAN_FACTORY_REGISTER_TOKEN(driver, simple0,
                                [](const boost::filesystem::path &location) {
                                  return single::driver::make_shared<driver>(
                                      location);
                                })
})

namespace {
template <typename Range>
test::Sequence::Order test_order(const Range &tests) {
  bool only_digits = true;
  for (const std::string &id : tests) {
    only_digits = only_digits && std::all_of(id.begin(), id.end(),
                                             boost::algorithm::is_digit());
    if (!only_digits) break;
  }
  return only_digits ? test::Sequence::NUMERIC
                     : test::Sequence::LEXICOGRAPHICAL;
}

test::Sequence::ContinueCondition test_continue_condition(
    const std::string &continue_condition_string) {
  test::Sequence::ContinueCondition continue_condition;
  if (!test::Sequence_ContinueCondition_Parse(continue_condition_string,
                                              &continue_condition)) {
    BOOST_THROW_EXCEPTION(
        invalid_continue_condition_error()
        << invalid_continue_condition_error::continue_condition(
            continue_condition_string));
  }
  return continue_condition;
}
}  // namespace

driver::driver(const boost::filesystem::path &location) : m_location(location) {
  boost::property_tree::read_ini((location / "config.ini").string(), m_config);
  // TODO
  read_system();
  read_info();
  read_tests();
  read_statement();
  read_settings();
  read_profiles();
  read_checker();
  read_interactor();
  m_overview.mutable_extension()->PackFrom(m_overview_extension);
}

void driver::read_system() {
  m_overview.mutable_system()->set_problem_type("to-be-set-later");
  m_overview.mutable_system()->set_package("to-be-set-later");
}

void driver::read_info() {
  Info &info = *m_overview.mutable_info();
  info.Clear();
  const boost::property_tree::ptree m_info = m_config.get_child("info");
  // name
  Info::Name &name = *info.add_name();
  name.set_language("C");
  name.set_value(m_info.get<std::string>("name"));
  // authors
  split::parse_repeated(*info.mutable_author(), m_info, "authors");
  // source
  boost::optional<std::string> value;
  if ((value = m_info.get_optional<std::string>("source")))
    info.set_source(value.get());
  // maintainers
  split::parse_repeated(*info.mutable_maintainer(), m_info, "maintainers");
}

static const std::string continue_condition_prefix = "continue_condition_";
static const std::string group_prefix = "group_";
static const std::string score_prefix = "score_";

void driver::read_tests() {
  boost::property_tree::ptree empty_tests_section;
  const boost::property_tree::ptree &tests_section =
      m_config.get_child("tests", empty_tests_section);
  m_tests = simple0::tests::instance(m_location / "tests", tests_section);

  // simple0-related restriction
  const auto data_set_ = m_tests->data_set();
  if (data_set_.find("in") == data_set_.end())
    BOOST_THROW_EXCEPTION(test_no_in_data_error());
  // note out is optional
  const std::size_t expected_size =
      data_set_.find("out") == data_set_.end() ? 1 : 2;
  if (data_set_.size() != expected_size)
    // TODO send at least first unknown data_id
    BOOST_THROW_EXCEPTION(test_unknown_data_error());

  // first initialize groups
  for (const auto kv : tests_section) {
    const std::string &key = kv.first;
    if (boost::algorithm::starts_with(key, group_prefix)) {
      const std::string group_id = key.substr(group_prefix.size());
      if (group_id.empty()) BOOST_THROW_EXCEPTION(empty_group_id_error());
      std::string tests = kv.second.get_value<std::string>();
      TestGroup &test_group = m_test_groups[group_id];
      test_group.set_id(group_id);
      std::vector<std::string> test_ids;
      boost::algorithm::split(test_ids, tests, boost::algorithm::is_space(),
                              boost::algorithm::token_compress_on);
      if (test_ids.size() == 1 && test_ids[0].empty()) test_ids.clear();
      test_group.mutable_tests()->set_order(test_order(test_ids));
      test_group.mutable_tests()->clear_query();
      for (const auto &test_id : test_ids) {
        test_group.mutable_tests()->add_query()->set_id(test_id);
      }
      test_group.mutable_tests()->set_continue_condition(
          test::Sequence::WHILE_OK);
    }
  }
  // only after initialization set values
  for (const auto kv : tests_section) {
    const std::string &key = kv.first;
    if (boost::algorithm::starts_with(key, score_prefix)) {
      const std::string group_id = key.substr(score_prefix.size());
      if (group_id.empty()) BOOST_THROW_EXCEPTION(empty_group_id_error());
      m_test_groups[group_id].set_score(kv.second.get_value<std::int64_t>());
    } else if (boost::algorithm::starts_with(key, continue_condition_prefix)) {
      const std::string group_id = key.substr(continue_condition_prefix.size());
      if (group_id.empty()) BOOST_THROW_EXCEPTION(empty_group_id_error());
      m_test_groups[group_id].mutable_tests()->set_continue_condition(
          test_continue_condition(kv.second.get_value<std::string>()));
    }
  }

  *m_overview_extension.mutable_tests() = m_tests->test_set_info();
  (*m_overview.mutable_utility())["tests"] = m_tests->info();
}

void driver::read_statement() {
  m_statement = statement::instance(m_location / "statement");
  Statement &info = *m_overview.mutable_statement() = m_statement->info();
  for (Statement::Version &v : *info.mutable_version()) {
    const bunsan::pm::entry package =
        bunsan::pm::entry("statement") / v.package();
    v.set_package(package.name());
  }
}

void driver::read_settings() {
  boost::optional<std::string> value;
  // resource limits
  bacs::process::ResourceLimits &resource_limits =
      *m_settings.mutable_resource_limits();
  if ((value = m_config.get_optional<std::string>("resource_limits.time")))
    resource_limits.set_time_limit_millis(
        resource::parse::time_millis(value.get()));
  if ((value = m_config.get_optional<std::string>("resource_limits.memory")))
    resource_limits.set_memory_limit_bytes(
        resource::parse::memory_bytes(value.get()));
  if ((value = m_config.get_optional<std::string>("resource_limits.output")))
    resource_limits.set_output_limit_bytes(
        resource::parse::memory_bytes(value.get()));
  // number of processes is not supported here
  if ((value = m_config.get_optional<std::string>("resource_limits.real_time")))
    resource_limits.set_real_time_limit_millis(
        resource::parse::time_millis(value.get()));
  // files & execution
  process::File *file = m_settings.add_file();
  process::Execution &execution = *m_settings.mutable_execution();
  file->set_id("stdin");
  file->set_init("in");
  file->add_permission(process::File::READ);
  if ((value = m_config.get_optional<std::string>("files.stdin"))) {
    bacs::file::path_convert(value.get(), *file->mutable_path());
  } else {
    process::Execution::Redirection &rd = *execution.add_redirection();
    rd.set_stream(process::Execution::Redirection::STDIN);
    rd.set_file_id("stdin");
  }
  file = m_settings.add_file();
  file->set_id("stdout");
  file->add_permission(process::File::READ);
  file->add_permission(process::File::WRITE);
  if ((value = m_config.get_optional<std::string>("files.stdout"))) {
    bacs::file::path_convert(value.get(), *file->mutable_path());
  } else {
    process::Execution::Redirection &rd = *execution.add_redirection();
    rd.set_stream(process::Execution::Redirection::STDOUT);
    rd.set_file_id("stdout");
  }
}

namespace {
void add_test_group_dependency(
    TestGroup &test_group,
    boost::optional<std::string> &dependency_test_group) {
  if (dependency_test_group) {
    Dependency &dependency = *test_group.add_dependency();
    dependency.set_test_group(*dependency_test_group);
    // FIXME dependency type?
  }
}
}  // namespace

void driver::read_profiles() {
  auto &profiles = *m_overview.mutable_profile();
  profiles.Clear();
  Profile &profile = *profiles.Add();
  ProfileExtension profile_extension;

  auto unused_tests = boost::copy_range<std::unordered_set<std::string>>(
      m_overview_extension.tests().test_set());

  boost::optional<std::string> previous_test_group;
  for (auto &group_tgroup : m_test_groups) {
    TestGroup &test_group = *profile_extension.add_test_group();
    test_group = std::move(group_tgroup.second);
    for (const test::Query &test_query : test_group.tests().query()) {
      unused_tests.erase(test_query.id());
    }
    *test_group.mutable_process() = m_settings;
    add_test_group_dependency(test_group, previous_test_group);
    previous_test_group = group_tgroup.first;
  }

  TestGroup &default_test_group = *profile_extension.add_test_group();
  default_test_group.mutable_tests()->set_order(test_order(unused_tests));
  for (const std::string &test_id : unused_tests) {
    default_test_group.mutable_tests()->add_query()->set_id(test_id);
  }
  default_test_group.mutable_tests()->set_continue_condition(
      test_continue_condition(
          m_config.get<std::string>("tests.continue_condition", "WHILE_OK")));
  *default_test_group.mutable_process() = m_settings;
  default_test_group.set_score(m_config.get<std::int64_t>("tests.score", 0));
  add_test_group_dependency(default_test_group, previous_test_group);

  profile.mutable_extension()->PackFrom(profile_extension);
}

void driver::read_checker() {
  if (boost::filesystem::exists(m_location / "checker")) {
    try {
      m_checker = utility::instance(m_location / "checker");
      (*m_overview.mutable_utility())["checker"] = m_checker->info();
    } catch (std::exception &) {
      BOOST_THROW_EXCEPTION(checker_error()
                            << checker_error::message(
                                   "Unable to initialize checker's driver.")
                            << bunsan::enable_nested_current());
    }
  }
}

void driver::read_interactor() {
  if (boost::filesystem::exists(m_location / "interactor")) {
    try {
      m_interactor = utility::instance(m_location / "interactor");
      (*m_overview.mutable_utility())["interactor"] = m_interactor->info();
    } catch (std::exception &) {
      BOOST_THROW_EXCEPTION(interactor_error()
                            << interactor_error::message(
                                   "Unable to initialize interactor's driver.")
                            << bunsan::enable_nested_current());
    }
  }
}

}  // namespace simple0
}  // namespace drivers
}  // namespace single
}  // namespace problem
}  // namespace bacs
