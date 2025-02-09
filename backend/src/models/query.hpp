#pragma once

#include <userver/formats/json.hpp>
#include <cstdint>
#include <chrono>
#include <string>
#include <vector>

namespace nl::models {

struct Query final {
    int32_t query_id_;
    std::string query_;
    std::string response_;
    std::chrono::system_clock::time_point created_at_;
};

userver::formats::json::Value Serialize(
    const Query& query,
    userver::formats::serialize::To<userver::formats::json::Value>);
}  // namespace nl::models