#include "note.hpp"

namespace nl::dto {

NoteRequest ParseNoteRequest(const userver::server::http::HttpRequest& request,
                                    userver::server::request::RequestContext& context) {
    NoteRequest noteRequest;
    noteRequest.user_id_ = context.GetData<int32_t>("user_id");

    if (request.HasPathArg("note_id")) {
        noteRequest.note_id_ = std::stoi(request.GetPathArg("note_id"));
        if (noteRequest.note_id_ < 1) {
            throw std::invalid_argument{"Invalid note_id: less 1"};
        }

    }
    userver::formats::json::Value request_body;
    try {
        request_body = userver::formats::json::FromString(request.RequestBody());
    } catch(...) {}

    if (request_body.HasMember("title")) {
        noteRequest.title_ = request_body["title"].As<std::string>();
        if (noteRequest.title_.size() < 1 || noteRequest.title_.size() > 512) {
            throw std::invalid_argument{"Invalid title size: empty or max(512) exeded"};
        }
    }
    if (request_body.HasMember("body")) {
        noteRequest.body_ = request_body["body"].As<std::string>();
        if (noteRequest.body_.size() > 20000) {
            throw std::invalid_argument{"Invalid body size: empty or max(20000) exeded"};
        }
    }
    return noteRequest;
}
} // namespace nl::dto




