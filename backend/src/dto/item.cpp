#include "item.hpp"
#include <optional>

namespace nl::dto {
    
ItemRequest ParseItemRequest(const userver::server::http::HttpRequest& request) {
    ItemRequest itemRequest;
    userver::formats::json::Value request_body;

    try {
        request_body = userver::formats::json::FromString(request.RequestBody());
    } catch (...) {}
    if (request_body.HasMember("text")) {
        itemRequest.text_ = request_body["text"].As<std::string>();
        if (itemRequest.text_.size() < 1 ||  itemRequest.text_.size() > 512) {
            throw std::invalid_argument("Text size error: less 1 or max(512) exeded");
        }
    }
    if (request_body.HasMember("status")) {
        itemRequest.status_.emplace(request_body["status"].As<bool>());
    }
    if (request.HasPathArg("item_id")) {
        itemRequest.item_id_ = std::stoi(request.GetPathArg("item_id"));
        if (itemRequest.item_id_ < 1) {
            throw std::invalid_argument{"Invalid item_id: less 1"};
        }
    }
    if (request.HasPathArg("checklist_id")) {
        itemRequest.checklist_id_ = std::stoi(request.GetPathArg("checklist_id"));
        if (itemRequest.checklist_id_  < 1) {
            throw std::invalid_argument{"Invalid checklist_id: less 1"};
        }
    }
    return itemRequest;
}

}  // namespace nl::dto  