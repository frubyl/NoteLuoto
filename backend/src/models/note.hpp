#pragma once

#include <userver/formats/json.hpp>
#include <cstdint>
#include <chrono>
#include <string>
#include <vector>

namespace nl::models {

struct NoteWithId final {
    int32_t note_id_;
    std::string title_;
    std::string body_;
    std::chrono::system_clock::time_point created_at_;
    std::chrono::system_clock::time_point updated_at_;

};
struct NoteWithoutId final {
    std::string title_;
    std::string body_;
    std::chrono::system_clock::time_point created_at_;
    std::chrono::system_clock::time_point updated_at_;
};

userver::formats::json::Value Serialize(
    const NoteWithId& noteWithId,
    userver::formats::serialize::To<userver::formats::json::Value>);

userver::formats::json::Value Serialize(
    const NoteWithoutId& noteWithoutId,
    userver::formats::serialize::To<userver::formats::json::Value>);
}  // namespace nl::models