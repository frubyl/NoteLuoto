#pragma once

#include <userver/formats/json.hpp>
#include <cstdint>
#include <chrono>
#include <string>
#include <vector>

namespace nl::models {

struct Item final {
    int32_t item_id_;
    std::string text_;
    bool completed_;
    std::chrono::system_clock::time_point created_at_;
    std::chrono::system_clock::time_point updated_at_;

};

struct Checklist final {
    std::string title_;
    std::chrono::system_clock::time_point created_at_;
    std::chrono::system_clock::time_point updated_at_;
};

struct ChecklistWithId final {
    int32_t checklist_id_;
    std::string title_;
    std::chrono::system_clock::time_point updated_at_;
};

userver::formats::json::Value Serialize(
    const Item& item,
    userver::formats::serialize::To<userver::formats::json::Value>);

userver::formats::json::Value Serialize(
    const Checklist& checklist,
    userver::formats::serialize::To<userver::formats::json::Value>);
}  // namespace nl::models