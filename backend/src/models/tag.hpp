#pragma once

#include <userver/formats/json.hpp>
#include <cstdint>
#include <string>

namespace nl::models {

struct Tag final {
    int32_t tag_id_;
    std::string name_;
};

userver::formats::json::Value Serialize(
    const Tag& tag,
    userver::formats::serialize::To<userver::formats::json::Value>);

}  // namespace nl::models