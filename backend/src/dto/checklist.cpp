#include "checklist.hpp"

namespace nl::dto {
    
ChecklistRequest ParseChecklistRequest(const userver::server::http::HttpRequest& request) {
    ChecklistRequest checklistRequest;
    userver::formats::json::Value request_body;
    try {
        request_body = userver::formats::json::FromString(request.RequestBody());
    } catch(...){}

    if (request_body.HasMember("title")) {
        checklistRequest.title_ = request_body["title"].As<std::string>();
        if (checklistRequest.title_.size() < 1 || checklistRequest.title_.size() > 256) {
            throw std::invalid_argument{"Title size error: less 1 or max(256) exeded"};
        }
    }
    if (request.HasPathArg("note_id")) {
        checklistRequest.note_id_ = std::stoi(request.GetPathArg("note_id"));
        if (checklistRequest.note_id_ < 1) {
            throw std::invalid_argument{"Invalid note_id: less 1"};
        }
    }
    if (request.HasPathArg("checklist_id")) {
        checklistRequest.checklist_id_ = std::stoi(request.GetPathArg("checklist_id"));
        if (checklistRequest.checklist_id_ < 1) {
            throw std::invalid_argument{"Invalid checklist_id: less 1"};
        }
    }
    return checklistRequest;
}

}  // namespace nl::dto  