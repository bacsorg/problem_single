#pragma once

#include "bunsan/pm/entry.hpp"

#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>

namespace bacs{namespace single{namespace problem
{
    /*!
     * \brief Something that may be build by bunsan::pm.
     *
     * \see \ref buildable_page
     */
    class buildable: private boost::noncopyable
    {
    public:
        virtual ~buildable();

    public:
        /// \return false if no package is needed
        virtual bool make_package(const boost::filesystem::path &destination,
                                  const bunsan::pm::entry &package) const=0;
    };
}}}
