#define BOOST_TEST_MODULE resource
#include <boost/test/unit_test.hpp>

#include "bacs/single/problem/detail/resource.hpp"

namespace de = bacs::single::problem::detail;

BOOST_AUTO_TEST_SUITE(resource)

BOOST_AUTO_TEST_CASE(parse_time_millis)
{
    BOOST_CHECK_EQUAL(de::parse_time_millis("1"), 1000);
    BOOST_CHECK_EQUAL(de::parse_time_millis("1s"), 1000);
    BOOST_CHECK_EQUAL(de::parse_time_millis("10s"), 10000);
    BOOST_CHECK_EQUAL(de::parse_time_millis("10ms"), 10);
    BOOST_CHECK_EQUAL(de::parse_time_millis("10hs"), 10 * 100 * 1000);
}

BOOST_AUTO_TEST_CASE(parse_memory_bytes)
{
    BOOST_CHECK_EQUAL(de::parse_memory_bytes("1"), 1);
    BOOST_CHECK_EQUAL(de::parse_memory_bytes("1024"), 1024);
    BOOST_CHECK_EQUAL(de::parse_memory_bytes("15B"), 15);
    BOOST_CHECK_EQUAL(de::parse_memory_bytes("1KiB"), 1024);
    BOOST_CHECK_EQUAL(de::parse_memory_bytes("1MiB"), 1024 * 1024);
}

BOOST_AUTO_TEST_SUITE_END() // resource
