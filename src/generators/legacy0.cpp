#include "legacy0.hpp"

#include <bacs/problem/single/problem.pb.h>

#include <bunsan/config/cast.hpp>
#include <bunsan/log/trivial.hpp>
#include <bunsan/filesystem/operations.hpp>
#include <bunsan/pm/index.hpp>
#include <bunsan/static_initializer.hpp>

#include <iconv.h>

#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/scope_exit.hpp>

#include <iomanip>
#include <sstream>
#include <unordered_map>

namespace bacs {
namespace problem {
namespace single {
namespace generators {

BUNSAN_STATIC_INITIALIZER(bacs_problem_single_generators_legacy0, {
  BUNSAN_FACTORY_REGISTER_TOKEN(
      generator, legacy0,
      [](const boost::property_tree::ptree &config) {
        return generator::make_shared<legacy0>(config);
      })
})

std::shared_ptr<db_connection> db_connection::instance(
    const options &options_) {
  {
    static std::once_flag once;
    std::call_once(once, mysql_library_init, 0, nullptr, nullptr);
  }
  static std::mutex lock;
  static std::unordered_map<std::string, std::shared_ptr<db_connection>> cache;
  {
    const std::lock_guard<std::mutex> lk(lock);
    const std::string key = options_.host + "#" +
                            boost::lexical_cast<std::string>(options_.port) +
                            "#" + options_.database;
    const auto iter = cache.find(key);
    if (iter == cache.end()) {
      return cache[key] = std::make_shared<db_connection>(options_);
    } else {
      return iter->second;
    }
  }
}

db_connection::db_connection(const options &options_) : m_impl(nullptr) {
  try {
    m_impl = mysql_init(nullptr);
    if (!m_impl) throw std::bad_alloc();
    mysql_options(m_impl, MYSQL_OPT_RECONNECT, "1");
    if (!mysql_real_connect(m_impl, options_.host.c_str(),
                            options_.user.c_str(), options_.password.c_str(),
                            options_.database.c_str(), options_.port,
                            /* unix socket */ nullptr,
                            /* flags */ 0))
      BOOST_THROW_EXCEPTION(mysql_error()
                            << mysql_error::error(::mysql_error(m_impl)));
  } catch (...) {
    if (m_impl) mysql_close(m_impl);
    throw;
  }
}

db_connection::~db_connection() {
  if (m_impl) mysql_close(m_impl);
}

std::string db_connection::escape(const std::string &s) {
  if (s.empty())  // workaround for iconv
    return s;
  const std::lock_guard<std::mutex> lk(m_lock);
  std::vector<char> buf(2 * s.size() + 1);
  const std::size_t size =
      mysql_real_escape_string(m_impl, buf.data(), s.data(), s.size());
  std::vector<char> buf2(2 * size);
  iconv_t ifd = (iconv_t)-1;
  BOOST_SCOPE_EXIT_ALL(ifd) {
    if (ifd != (iconv_t)-1) iconv_close(ifd);
  };
  ifd = iconv_open("cp1251", "utf8");
  if (ifd != (iconv_t)-1) {
    char *inbuf = buf.data();
    char *outbuf = buf2.data();
    std::size_t inbytesleft = size;
    std::size_t outbytesleft = buf2.size();
    iconv(ifd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
    return std::string(buf2.data(), outbuf);
  } else {
    return std::string(buf.data(), size);
  }
}

void db_connection::query(const std::string &q) {
  const std::lock_guard<std::mutex> lk(m_lock);
  const int ret = mysql_real_query(m_impl, q.data(), q.size());
  if (ret)
    BOOST_THROW_EXCEPTION(mysql_error()
                          << mysql_error::query(q)
                          << mysql_error::error(::mysql_error(m_impl)));
}

legacy0::legacy0(const boost::property_tree::ptree &config_)
    : m_config(bunsan::config::load<config>(config_)),
      m_internal0(generator::instance("internal0", m_config.internal0)),
      m_connection(db_connection::instance(m_config.db)) {}

Problem legacy0::generate(const options &options_) {
  const Problem problem_info = m_internal0->generate(options_);
  const Profile &profile = problem_info.profile(0);
  ProfileExtension profile_extension;
  BOOST_VERIFY(profile.extension().UnpackTo(&profile_extension));
  // prepare sources
  const bunsan::pm::entry root_package = options_.root_package / "legacy0";
  const boost::filesystem::path package_root = options_.destination / "legacy0";
  BOOST_VERIFY(boost::filesystem::create_directory(package_root));
  bunsan::pm::index internal0_index;
  internal0_index.load(options_.destination / "index");
  bunsan::pm::index root_index;
  boost::property_tree::ptree problem_config;
  // root package
  {
    const boost::filesystem::path package_src = package_root / "pkg";
    root_index.package.self.push_back({".", "pkg"});
    boost::filesystem::create_directory(package_src);
    // configuration variables
    std::string problem_id =
                    options_.root_package.location().filename().string(),
                problem_name, problem_author, problem_text, problem_input,
                problem_output;
    double problem_tl, problem_ml;
    // configuration
    // name
    if (problem_info.info().name_size() < 1)
      BOOST_THROW_EXCEPTION(invalid_problem_error());
    problem_config.put("name",
                       problem_name = problem_info.info().name(0).value());
    // maintainer
    if (problem_info.info().maintainer_size() < 1)
      BOOST_THROW_EXCEPTION(invalid_problem_error());
    problem_author = problem_info.info().maintainer(0);
    // resources
    if (problem_info.profile_size() < 1)
      BOOST_THROW_EXCEPTION(invalid_problem_error());
    if (profile_extension.test_group_size() < 1)
      BOOST_THROW_EXCEPTION(invalid_problem_error());
    // time limit
    problem_tl = profile_extension.test_group(0)
                     .process()
                     .resource_limits()
                     .time_limit_millis() /
                 1000.;
    problem_config.put("TL", problem_tl);
    // memory limit
    problem_ml = profile_extension.test_group(0)
                     .process()
                     .resource_limits()
                     .memory_limit_bytes() /
                 1024. / 1024.;
    problem_config.put("ML", problem_ml);
    // files
    for (const process::File &file :
         profile_extension.test_group(0).process().file()) {
      if (file.id().substr(0, 3) == "std" && file.has_path())
        problem_config.put(file.id().substr(3) + "put",
                           file.path().element(file.path().element_size() - 1));
    }
    problem_input = problem_config.get("input", "STDIN");
    problem_output = problem_config.get("output", "STDOUT");
    // checker
    const utility_ptr checker = options_.driver->checker();
    const std::string checker_call =
        checker ? checker->section("utility").get<std::string>("call") : "std";
    const boost::optional<std::string> checker_return =
        checker ? checker->section("utility").get<std::string>("return", "none")
                : boost::optional<std::string>(boost::none);
    if (checker_call == "std") {
      problem_config.put("checker", "default");
    } else if (checker_call == "in_out_hint") {
      boost::filesystem::create_directory(package_src / "checker");
      boost::filesystem::create_symlink("../bin/checker",
                                        package_src / "checker" / "check_new");
      problem_config.put("checker", "check_new");
      if (!checker_return) BOOST_THROW_EXCEPTION(invalid_problem_error());
      problem_config.put("checker_return", *checker_return);
      root_index.package.import.package.push_back(
          {".", options_.root_package / "checker"});
    }
    boost::filesystem::create_symlink("share/tests", package_src / "tests");
    // tests
    if (boost::filesystem::exists(options_.destination / "tests" / "index"))
      root_index.package.import.package.push_back(
          {".", options_.root_package / "tests"});
    // statement
    {
      const std::unordered_map<std::string, int> format_priority = {
          {"pdf", 1000},
          {"ps", 750},
          {"odt", 600},
          {"html", 500},
          {"htm", 500},
          {"xhtml", 500},
          {"dvi", 500},
          {"rtf", 500},
          {"doc", 400},
          {"docx", 300},
          {"txt", 250},
          {"tex", 0}};
      int prior = -1;
      for (const Statement::Version &version :
           problem_info.statement().version()) {
        const auto iter = format_priority.find(version.format());
        if (iter != format_priority.end() && iter->second > prior) {
          prior = iter->second;
          problem_text = version.package();
        }
      }
    }
    // database update
    {
      std::ostringstream sout;
      sout << "insert into nproblem (problem_id, problem_name, "
              "                      author, text, "
              "                      time_limit, memory_limit, "
              "                      status, input, output, revision) "
           << "values('" << m_connection->escape(problem_id) << "', '"
           << m_connection->escape(problem_name) << "', '"
           << m_connection->escape(problem_author) << "', '"
           << m_connection->escape(problem_text) << "', " << std::fixed
           << problem_tl << ", " << std::fixed << problem_ml << ", " << 1
           << ", '" << m_connection->escape(problem_input) << "', '"
           << m_connection->escape(problem_output) << "', '"
           << m_connection->escape(
                  problem_info.system().revision().ShortDebugString()) << "') "
           << "on duplicate key update "
           << "problem_name='" << m_connection->escape(problem_name) << "', "
           << "author='" << m_connection->escape(problem_author) << "', "
           << "text='" << m_connection->escape(problem_text) << "', "
           << "time_limit=" << std::fixed << problem_tl << ", "
           << "memory_limit=" << std::fixed << problem_ml << ", "
           << "input='" << m_connection->escape(problem_input) << "', "
           << "output='" << m_connection->escape(problem_output) << "', "
           << "revision='"
           << m_connection->escape(
                  problem_info.system().revision().ShortDebugString()) << "'";
      BUNSAN_LOG_DEBUG << "Attempt to make SQL query: " << sout.str();
      m_connection->query(sout.str());
    }
  }
  // the last command
  boost::property_tree::write_ini(
      (package_root / "pkg" / "conf.txt").string().c_str(), problem_config);
  root_index.save(package_root / "index");
  return problem_info;
}

}  // namespace generators
}  // namespace single
}  // namespace problem
}  // namespace bacs
