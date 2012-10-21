#pragma once

#include "bunsan/pm/entry.hpp"

#include <boost/filesystem/path.hpp>

namespace bacs{namespace single{namespace problem
{
    class buildable
    {
    public:
        virtual ~buildable();

    public:
        virtual void make_package(const boost::filesystem::path &destination,
                                  const bunsan::pm::entry &package)=0;
    };
}}}
