#include "bacs/problem/single/detail/path.hpp"

namespace bacs{namespace problem{namespace single{namespace detail
{
    void to_pb_path(const boost::filesystem::path &path, settings::Path &pb_path)
    {
        pb_path.Clear();
        if (path.has_root_directory())
            *pb_path.mutable_root() = path.root_directory().string();
        for (const boost::filesystem::path &r: path.relative_path())
            *pb_path.add_elements() = r.string();
    }
}}}}
