#include "checklist.hpp"

namespace nl::dto {
    
ChecklistRequest ParseChecklistRequest(const userver::server::http::HttpRequest& request) {
    ChecklistRequest checklistRequest;
    try {
        auto request_body = userver::formats::json::FromString(request.RequestBody());
        if (request_body.HasMember("title")) {
            checklistRequest.title_ = request_body["title"].As<std::string>();
        }
    } catch(...){}
    if (request.HasPathArg("note_id")) {
        checklistRequest.note_id_ = std::stoi(request.GetPathArg("note_id"));
    }
    if (request.HasPathArg("checklist_id")) {
        checklistRequest.checklist_id_ = std::stoi(request.GetPathArg("checklist_id"));

    }
    return checklistRequest;
}

}  // namespace nl::dto  