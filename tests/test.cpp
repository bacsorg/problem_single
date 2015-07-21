#define BOOST_TEST_MODULE test
#include <boost/test/unit_test.hpp>

#include <bacs/problem/single/test/matcher.hpp>

namespace bpst = bacs::problem::single::test;

BOOST_AUTO_TEST_SUITE(test)

BOOST_AUTO_TEST_SUITE(matcher)

BOOST_AUTO_TEST_CASE(id) {
  bpst::Query query;
  query.set_id("hello");
  const bpst::matcher matcher(query);
  BOOST_CHECK(matcher("hello"));
  BOOST_CHECK(!matcher("world"));
  BOOST_CHECK(!matcher(""));
}

BOOST_AUTO_TEST_CASE(wildcard) {
  bpst::Query query;
  query.mutable_wildcard()->set_value("some*");
  const bpst::matcher matcher(query);
  BOOST_CHECK(matcher("some"));
  BOOST_CHECK(matcher("something"));
  BOOST_CHECK(!matcher("SOMETHING"));
  BOOST_CHECK(!matcher("notsome"));
  BOOST_CHECK(!matcher(""));
}

BOOST_AUTO_TEST_CASE(iwildcard) {
  bpst::Query query;
  query.mutable_wildcard()->set_value("some*");
  query.mutable_wildcard()->add_flag(bpst::WildcardQuery::IGNORE_CASE);
  const bpst::matcher matcher(query);
  BOOST_CHECK(matcher("some"));
  BOOST_CHECK(matcher("something"));
  BOOST_CHECK(matcher("SOMETHING"));
  BOOST_CHECK(!matcher("notsome"));
  BOOST_CHECK(!matcher(""));
}

BOOST_AUTO_TEST_CASE(regex) {
  bpst::Query query;
  query.mutable_regex()->set_value("some.*");
  const bpst::matcher matcher(query);
  BOOST_CHECK(matcher("some"));
  BOOST_CHECK(matcher("something"));
  BOOST_CHECK(!matcher("SOMETHING"));
  BOOST_CHECK(!matcher("notsome"));
  BOOST_CHECK(!matcher(""));
}

BOOST_AUTO_TEST_CASE(iregex) {
  bpst::Query query;
  query.mutable_regex()->set_value("some.*");
  query.mutable_regex()->add_flag(bpst::RegexQuery::IGNORE_CASE);
  const bpst::matcher matcher(query);
  BOOST_CHECK(matcher("some"));
  BOOST_CHECK(matcher("something"));
  BOOST_CHECK(matcher("SOMETHING"));
  BOOST_CHECK(!matcher("notsome"));
  BOOST_CHECK(!matcher(""));
}

BOOST_AUTO_TEST_SUITE_END()  // matcher

BOOST_AUTO_TEST_SUITE_END()  // tests
