#include "bunsan/config.hpp"

#include "simple0_tests.hpp"

#include "bunsan/enable_error_info.hpp"
#include "bunsan/filesystem/fstream.hpp"
#include "bunsan/pm/index.hpp"

#include <unordered_map>

#include <boost/archive/text_oarchive.hpp>
#include "bunsan/serialization/unordered_set.hpp"

#include <boost/filesystem/operations.hpp>

namespace bacs{namespace problem{namespace single{namespace drivers
{
    static const boost::property_tree::ptree config =
        []()
        {
            boost::property_tree::ptree config;
            config.put("build.builder", "[simple0 tests generator]");
            config.put("utility.call", "copy");
            return config;
        }();

    simple0_tests::simple0_tests(const boost::filesystem::path &location,
                                 const std::unordered_set<std::string> &text_data_set):
        tests(location, config),
        m_text_data_set(text_data_set)
    {
        std::unordered_map<std::string, std::unordered_set<std::string>> test_files;
        for (boost::filesystem::directory_iterator i(location), end; i != end; ++i)
        {
            if (!boost::filesystem::is_regular_file(i->path()))
                BOOST_THROW_EXCEPTION(test_format_error());
            const std::string fname = i->path().filename().string();
            const std::string::size_type dot = fname.find('.');
            if (dot == std::string::npos)
                BOOST_THROW_EXCEPTION(test_format_error());
            if (fname.find('.', dot + 1) != std::string::npos)
                BOOST_THROW_EXCEPTION(test_format_error());
            const std::string test_id = fname.substr(0, dot);
            const std::string data_id = fname.substr(dot + 1);
            m_test_set.insert(test_id);
            m_data_set.insert(data_id);
            test_files[test_id].insert(data_id);
        }
        for (const auto &test_info: test_files)
            if (test_info.second != m_data_set)
                BOOST_THROW_EXCEPTION(test_format_error());
        // simple0-related restriction
        if (m_data_set.find("in") == m_data_set.end())
            BOOST_THROW_EXCEPTION(test_no_in_data_error());
        if (m_data_set.find("out") == m_data_set.end())
            BOOST_THROW_EXCEPTION(test_no_out_data_error());
        if (m_data_set.size() != 2)
            BOOST_THROW_EXCEPTION(test_unknown_data_error());
    }

    bool simple0_tests::make_package(const boost::filesystem::path &destination,
                                     const bunsan::pm::entry &/*package*/) const
    {
        boost::filesystem::create_directories(destination);
        bunsan::pm::index index;
        // tests builder
        index.source.import.package.insert(std::make_pair(".", "bacs/system/single/driver/simple0/tests"));
        // tests
        index.source.self.insert(std::make_pair("share/tests", "tests"));
        boost::filesystem::create_directory(destination / "tests");
        {
            for (boost::filesystem::directory_iterator i(location()), end; i != end; ++i)
                boost::filesystem::copy_file(i->path(), destination / "tests" / i->path().filename());
        }
        // configuration for tests generator
        index.source.self.insert(std::make_pair("etc", "etc"));
        boost::filesystem::create_directory(destination / "etc");
        BUNSAN_EXCEPTIONS_WRAP_BEGIN()
        {
            bunsan::filesystem::ofstream fout(destination / "etc/tests");
            {
                boost::archive::text_oarchive oa(fout);
                // FIXME I don't like that order should be checked by programmer.
                // Should be moved into separate header.
                oa << m_test_set << m_data_set << m_text_data_set;
            }
            fout.close();
        }
        BUNSAN_EXCEPTIONS_WRAP_END()
        index.save(destination / "index");
        return true;
    }

    std::unordered_set<std::string> simple0_tests::data_set() const
    {
        return m_data_set;
    }

    std::unordered_set<std::string> simple0_tests::test_set() const
    {
        return m_test_set;
    }
}}}}
