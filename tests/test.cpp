#define BOOST_TEST_MODULE test
#include <boost/test/unit_test.hpp>

#include <bacs/problem/single/test/matcher.hpp>

namespace bps = bacs::problem::single;
namespace bpst = bps::test;

BOOST_AUTO_TEST_SUITE(test)

BOOST_AUTO_TEST_SUITE(matcher)

BOOST_AUTO_TEST_CASE(id) {
  bps::TestQuery query;
  query.set_id("hello");
  const bpst::matcher matcher(query);
  BOOST_CHECK(matcher("hello"));
  BOOST_CHECK(!matcher("world"));
  BOOST_CHECK(!matcher(""));
}

BOOST_AUTO_TEST_CASE(copy) {
  bps::TestQuery query;
  query.set_id("copy");
  const bpst::matcher matcher(query);
  BOOST_CHECK(matcher("copy"));
  BOOST_CHECK(!matcher("not copy"));
  const bpst::matcher matcher2 = matcher;
  BOOST_CHECK(matcher2("copy"));
  BOOST_CHECK(!matcher2("not copy"));
}

BOOST_AUTO_TEST_CASE(wildcard) {
  bps::TestQuery query;
  query.mutable_wildcard()->set_value("some*");
  const bpst::matcher matcher(query);
  BOOST_CHECK(matcher("some"));
  BOOST_CHECK(matcher("something"));
  BOOST_CHECK(!matcher("SOMETHING"));
  BOOST_CHECK(!matcher("notsome"));
  BOOST_CHECK(!matcher(""));
}

BOOST_AUTO_TEST_CASE(iwildcard) {
  bps::TestQuery query;
  query.mutable_wildcard()->set_value("some*");
  query.mutable_wildcard()->add_flag(bps::TestQuery::Wildcard::IGNORE_CASE);
  const bpst::matcher matcher(query);
  BOOST_CHECK(matcher("some"));
  BOOST_CHECK(matcher("something"));
  BOOST_CHECK(matcher("SOMETHING"));
  BOOST_CHECK(!matcher("notsome"));
  BOOST_CHECK(!matcher(""));
}

BOOST_AUTO_TEST_CASE(regex) {
  bps::TestQuery query;
  query.mutable_regex()->set_value("some.*");
  const bpst::matcher matcher(query);
  BOOST_CHECK(matcher("some"));
  BOOST_CHECK(matcher("something"));
  BOOST_CHECK(!matcher("SOMETHING"));
  BOOST_CHECK(!matcher("notsome"));
  BOOST_CHECK(!matcher(""));
}

BOOST_AUTO_TEST_CASE(iregex) {
  bps::TestQuery query;
  query.mutable_regex()->set_value("some.*");
  query.mutable_regex()->add_flag(bps::TestQuery::Regex::IGNORE_CASE);
  const bpst::matcher matcher(query);
  BOOST_CHECK(matcher("some"));
  BOOST_CHECK(matcher("something"));
  BOOST_CHECK(matcher("SOMETHING"));
  BOOST_CHECK(!matcher("notsome"));
  BOOST_CHECK(!matcher(""));
}

BOOST_AUTO_TEST_SUITE_END()  // matcher

BOOST_AUTO_TEST_SUITE_END()  // tests
