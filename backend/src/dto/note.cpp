#include "note.hpp"

namespace nl::dto {

NoteRequest ParseNoteRequest(const userver::server::http::HttpRequest& request,
                                    userver::server::request::RequestContext& context) {
    NoteRequest noteRequest;
    auto request_body = userver::formats::json::FromString(request.RequestBody());
    noteRequest.user_id_ = context.GetData<int32_t>("user_id");

    if (request.HasPathArg("note_id")) {
        noteRequest.note_id_ = std::stoi(request.GetPathArg("note_id"));
    }
    if (request_body.HasMember("title")) {
        noteRequest.title_ = request_body["title"].As<std::string>();
    }
    if (request_body.HasMember("body")) {
        noteRequest.body_ = request_body["body"].As<std::string>();
    }
    return noteRequest;
}
} // namespace nl::dto




