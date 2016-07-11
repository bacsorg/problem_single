#include <bacs/problem/single/test/matcher.hpp>

#include <bunsan/fnmatch.hpp>

#include <boost/regex.hpp>

namespace bacs {
namespace problem {
namespace single {
namespace test {

class matcher::impl {
 public:
  virtual ~impl() {}
  virtual bool match(const std::string &test_id) const = 0;
};

class matcher::id : public matcher::impl {
 public:
  explicit id(const std::string &test_id) : m_test_id(test_id) {}

  bool match(const std::string &test_id) const override {
    return test_id == m_test_id;
  }

 private:
  const std::string m_test_id;
};

class matcher::wildcard : public matcher::impl {
 public:
  explicit wildcard(const TestQuery::Wildcard &query)
      : m_wildcard(query.value(), flags(query)) {}

  bool match(const std::string &test_id) const override {
    return m_wildcard(test_id);
  }

 private:
  bunsan::fnmatcher::flag flags(const TestQuery::Wildcard &query) {
    bunsan::fnmatcher::flag flags_ = bunsan::fnmatcher::defaults;
    for (const int flag : query.flag()) {
      switch (static_cast<TestQuery::Wildcard::Flag>(flag)) {
        case problem::single::TestQuery::Wildcard::IGNORE_CASE:
          flags_ |= bunsan::fnmatcher::icase;
          break;
      }
    }
    return flags_;
  }

 private:
  const bunsan::fnmatcher m_wildcard;
};

class matcher::regex : public matcher::impl {
 public:
  explicit regex(const TestQuery::Regex &query)
      : m_regex(query.value(), flags(query)) {}

  bool match(const std::string &test_id) const override {
    return boost::regex_match(test_id, m_regex);
  }

 private:
  boost::regex_constants::syntax_option_type flags(
      const problem::single::TestQuery::Regex &query) {
    boost::regex_constants::syntax_option_type flags_ =
        boost::regex_constants::normal;
    for (const int flag : query.flag()) {
      switch (static_cast<TestQuery::Regex::Flag>(flag)) {
        case TestQuery::Regex::IGNORE_CASE:
          flags_ |= boost::regex_constants::icase;
          break;
      }
    }
    return flags_;
  }

 private:
  const boost::regex m_regex;
};

matcher::matcher(const TestQuery &query) : m_impl(make_query(query)) {}

matcher::~matcher() {}

bool matcher::operator()(const std::string &test_id) const {
  BOOST_ASSERT(m_impl);
  return m_impl->match(test_id);
}

std::shared_ptr<const matcher::impl> matcher::make_query(
    const TestQuery &query) {
  switch (query.query_case()) {
    case TestQuery::kId:
      return std::make_shared<id>(query.id());
    case TestQuery::kWildcard:
      return std::make_shared<wildcard>(query.wildcard());
    case TestQuery::kRegex:
      return std::make_shared<regex>(query.regex());
    default:
      BOOST_THROW_EXCEPTION(matcher_not_set_error());
      return nullptr;
  }
}

}  // namespace test
}  // namespace single
}  // namespace problem
}  // namespace bacs
