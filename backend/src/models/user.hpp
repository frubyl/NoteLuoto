#pragma once

#include <userver/formats/json.hpp>
#include <cstdint>
#include <chrono>
#include <string>

namespace nl::models {

struct User final {
    int32_t user_id_;
    std::string username_;
    std::string password_hash_;
    std::chrono::system_clock::time_point created_at_;
};
}  // namespace nl::models