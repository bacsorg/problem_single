#include "single.hpp"

#include "bacs/single/problem/detail/split.hpp"

#include "bunsan/filesystem/operations.hpp"
#include "bunsan/system_error.hpp"
#include "bunsan/pm/depends.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace bacs{namespace single{namespace problem{namespace utilities
{
    bool single::factory_reg_hook = utility::register_new("single",
        [](const boost::filesystem::path &location,
           const boost::property_tree::ptree &config)
        {
            utility_ptr tmp(new single(location, config));
            return tmp;
        });

    single::single(const boost::filesystem::path &location,
                   const boost::property_tree::ptree &config):
        utility(location, config),
        m_source(config.get<std::string>("build.source")),
        m_std(config.get_optional<std::string>("build.std")),
        m_libs(detail::get_vector(config, "build.libs")) {}

    namespace
    {
        std::string get_lang(const std::string &source)
        {
            const std::string::size_type dot = source.rfind('.');
            if (dot == std::string::npos)
                return "unknown";
            const std::string suff = source.substr(dot + 1);
            if (suff == "cpp" || suff == "cc")
                return "cpp";
            else if (suff == "pas" || suff == "dpr")
                return "pas";
            else if (suff == "java")
                return "java";
            else
                return "unknown";
        }
    }

    bool single::make_package(const boost::filesystem::path &destination,
                              const bunsan::pm::entry &/*package*/)
    {
        boost::filesystem::create_directories(destination);
        bunsan::pm::depends index;
        const std::string lang = get_lang(m_source.filename().string());
        // builder itself
        index.source.import.source.insert(std::make_pair(".", "bacs/system/utility/single/" + lang));
        // sources, note: only build section is needed from config
        index.source.self.insert(std::make_pair("src", "src"));
        bunsan::filesystem::copy_tree(location(), destination / "src");
        // modules: set binary name
        index.source.self.insert(std::make_pair("modules", "modules"));
        boost::filesystem::create_directory(destination / "modules");
        {
            boost::filesystem::ofstream fout(destination / "modules" / "utility.cmake");
            if (fout.bad())
                BOOST_THROW_EXCEPTION(bunsan::system_error("open"));
            fout << "set(target " << target().string() << ")\n";
            fout << "set(source " << m_source.string() << ")\n";
            fout << "set(libraries";
            for (const std::string &lib: m_libs)
                fout << " " << lib;
            fout << ")\n";
            if (m_std)
                fout << "set(std " << m_std << ")\n";
            if (fout.bad())
                BOOST_THROW_EXCEPTION(bunsan::system_error("write"));
            fout.close();
            if (fout.bad())
                BOOST_THROW_EXCEPTION(bunsan::system_error("close"));
        }
        // dependencies
        for (const std::string &lib: m_libs)
            index.source.import.package.insert(std::make_pair(".", "bacs/lib/" + lang + "/" + lib));
        // save it
        index.save(destination / "index");
        return true;
    }
}}}}
