#pragma once

#include <userver/formats/json.hpp>
#include <cstdint>
#include <chrono>
#include <string>
#include <userver/server/http/http_request.hpp>

namespace nl::dto {

struct ChecklistRequest final {
    int32_t note_id_;
    int32_t checklist_id_;
    std::string title_;
};

ChecklistRequest ParseChecklistRequest(const userver::server::http::HttpRequest& request);


}  // namespace nl::dto  

