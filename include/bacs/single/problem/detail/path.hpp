#pragma once

#include "bacs/single/api/settings.pb.h"

#include <boost/filesystem/path.hpp>

namespace bacs{namespace single{namespace problem{namespace detail
{
    void to_pb_path(const boost::filesystem::path &path, api::pb::settings::Path &pb_path);
}}}}
