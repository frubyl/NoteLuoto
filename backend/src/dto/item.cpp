#include "item.hpp"
#include <optional>

namespace nl::dto {
    
ItemRequest ParseItemRequest(const userver::server::http::HttpRequest& request) {
    ItemRequest itemRequest;
    try {
        auto request_body = userver::formats::json::FromString(request.RequestBody());
        if (request_body.HasMember("text")) {
            itemRequest.text_.emplace(request_body["text"].As<std::string>());
        }
        if (request_body.HasMember("status")) {
            itemRequest.status_.emplace(request_body["status"].As<bool>());
        }
    } catch (...) {}
    if (request.HasPathArg("item_id")) {
        itemRequest.item_id_ = std::stoi(request.GetPathArg("item_id"));
    }
    if (request.HasPathArg("checklist_id")) {
        itemRequest.checklist_id_ = std::stoi(request.GetPathArg("checklist_id"));

    }
    return itemRequest;
}

}  // namespace nl::dto  