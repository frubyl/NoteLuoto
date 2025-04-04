#pragma once

#include <userver/formats/json.hpp>
#include <cstdint>
#include <optional>
#include <string>
#include <userver/server/http/http_request.hpp>

namespace nl::dto {

struct ItemRequest final {
    int32_t item_id_;
    int32_t checklist_id_;
    std::string text_;
    std::optional<bool> status_;
};

ItemRequest ParseItemRequest(const userver::server::http::HttpRequest& request);


}  // namespace nl::dto  

