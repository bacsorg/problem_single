#include <bunsan/config.hpp>

#include <bacs/problem/single/detail/list_tests.hpp>

#include <bunsan/filesystem/fstream.hpp>
#include <bunsan/pm/index.hpp>
#include <bunsan/range/construct_from_range.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/range/adaptor/map.hpp>
#include <bunsan/serialization/unordered_set.hpp>

namespace bacs{namespace problem{namespace single{namespace detail
{
    boost::property_tree::ptree get_config(const std::string &builder_name)
    {
        boost::property_tree::ptree config;
        config.put("build.builder", builder_name);
        config.put("utility.call", "copy");
        return config;
    }

    namespace
    {
        template <typename T>
        std::unordered_set<std::string> map_keys(
            const std::unordered_map<std::string, T> &map)
        {
            return bunsan::range::construct_from_range<
                std::unordered_set<std::string>>(
                    map | boost::adaptors::map_keys);
        }
    }

    list_tests::list_tests(const boost::filesystem::path &location,
                           const std::string &builder_name):
        tests(location, get_config(builder_name)) {}

    list_tests::list_tests(const boost::filesystem::path &location):
        list_tests(location, "[list_tests tests generator]") {}

    void list_tests::add_test(const std::string &test_id,
                              const test_data &data)
    {
        if (!m_tests.empty())
        {
            const auto &test = *m_tests.begin();
            if (map_keys(data) != map_keys(test.second))
                BOOST_THROW_EXCEPTION(inconsistent_test_data_set_error());
        }
        const auto iter = m_tests.find(test_id);
        if (iter == m_tests.end())
        {
            m_tests[test_id] = data;
            for (const auto &id_file: data)
                text_data(id_file.first);
        }
        else
        {
            if (iter->second != data)
                BOOST_THROW_EXCEPTION(inconsistent_test_data_error() <<
                                      inconsistent_test_data_error::message(
                                          "Tests with the same id "
                                          "have different data files."));
        }
    }

    void list_tests::text_data(const std::string &data_id, const bool value)
    {
        if (m_tests.empty())
            BOOST_THROW_EXCEPTION(test_empty_set_error());
        if (value)
        {
            if (m_tests.begin()->second.find(data_id) == m_tests.begin()->second.end())
                BOOST_THROW_EXCEPTION(test_unknown_data_error() <<
                                      test_unknown_data_error::data_id(data_id));
            m_text_data_set.insert(data_id);
        }
        else
        {
            m_text_data_set.erase(data_id);
        }
    }

    std::unordered_set<std::string> list_tests::data_set() const
    {
        if (m_tests.empty())
            BOOST_THROW_EXCEPTION(test_empty_set_error());
        return map_keys(m_tests.begin()->second);
    }

    std::unordered_set<std::string> list_tests::test_set() const
    {
        return map_keys(m_tests);
    }

    bool list_tests::make_package(const boost::filesystem::path &destination,
                                  const bunsan::pm::entry &/*package*/) const
    {
        boost::filesystem::create_directories(destination);
        bunsan::pm::index index;
        // tests builder
        index.source.import.package.insert(std::make_pair(".", "bacs/system/single/list_tests"));
        // tests
        index.source.self.insert(std::make_pair("share/tests", "tests"));
        boost::filesystem::create_directory(destination / "tests");
        for (const auto &id_data: m_tests)
            for (const auto &id_path: id_data.second)
                boost::filesystem::copy_file(location() / id_path.second, destination / "tests" / (id_data.first + "." + id_path.first));
        // configuration for tests generator
        index.source.self.insert(std::make_pair("etc", "etc"));
        boost::filesystem::create_directory(destination / "etc");
        bunsan::filesystem::ofstream fout(destination / "etc/tests");
        BUNSAN_FILESYSTEM_FSTREAM_WRAP_BEGIN(fout)
        {
            {
                // note: test_set_ and data_set_ are not altered, but boost::oarchive interface
                // does not support const references in operator<<()
                std::unordered_set<std::string> test_set_ = test_set(), data_set_ = data_set();
                boost::archive::text_oarchive oa(fout);
                // FIXME I don't like that order should be checked by programmer.
                // Should be moved into separate header.
                oa << test_set_ << data_set_ << m_text_data_set;
            }
        }
        BUNSAN_FILESYSTEM_FSTREAM_WRAP_END(fout)
        fout.close();
        index.save(destination / "index");
        return true;
    }
}}}}
