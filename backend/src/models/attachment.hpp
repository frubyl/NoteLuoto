#pragma once

#include <userver/formats/json.hpp>
#include <string>

namespace nl::models {

struct Attachment final {
    std::string file_name_;
    std::string old_file_name_;
};

}  // namespace nl::models