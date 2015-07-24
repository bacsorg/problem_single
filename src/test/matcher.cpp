#include <bacs/problem/single/test/matcher.hpp>

#include <boost/regex.hpp>

#include <fnmatch.h>

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
  explicit wildcard(const WildcardQuery &query)
      : m_wildcard(query.value()), m_flags(flags(query)) {}

  bool match(const std::string &test_id) const override {
    const int m = ::fnmatch(m_wildcard.c_str(), test_id.c_str(), m_flags);
    if (m && m != FNM_NOMATCH)
      BOOST_THROW_EXCEPTION(matcher_error()
                            << matcher_error::message("fnmatch() has failed"));
    return !m;
  }

 private:
  int flags(const WildcardQuery &query) {
    int flags_ = 0;
    for (const int flag : query.flag()) {
      switch (static_cast<WildcardQuery::Flag>(flag)) {
        case problem::single::test::WildcardQuery::IGNORE_CASE:
          flags_ |= FNM_CASEFOLD;
          break;
      }
    }
    return flags_;
  }

 private:
  const std::string m_wildcard;
  int m_flags;
};

class matcher::regex : public matcher::impl {
 public:
  explicit regex(const RegexQuery &query)
      : m_regex(query.value(), flags(query)) {}

  bool match(const std::string &test_id) const override {
    return boost::regex_match(test_id, m_regex);
  }

 private:
  boost::regex_constants::syntax_option_type flags(
      const problem::single::test::RegexQuery &query) {
    boost::regex_constants::syntax_option_type flags_ =
        boost::regex_constants::normal;
    for (const int flag : query.flag()) {
      switch (static_cast<RegexQuery::Flag>(flag)) {
        case RegexQuery::IGNORE_CASE:
          flags_ |= boost::regex_constants::icase;
          break;
      }
    }
    return flags_;
  }

 private:
  const boost::regex m_regex;
};

matcher::matcher(const Query &query) : m_impl(make_query(query)) {}

matcher::~matcher() {}

bool matcher::operator()(const std::string &test_id) const {
  BOOST_ASSERT(m_impl);
  return m_impl->match(test_id);
}

std::shared_ptr<const matcher::impl> matcher::make_query(const Query &query) {
  switch (query.query_case()) {
    case Query::kId:
      return std::make_shared<id>(query.id());
    case Query::kWildcard:
      return std::make_shared<wildcard>(query.wildcard());
    case Query::kRegex:
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
