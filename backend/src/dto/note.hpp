#pragma once

#include <userver/formats/json.hpp>
#include <cstdint>
#include <string>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
namespace nl::dto {

struct NoteRequest final {
    int32_t user_id_;
    int32_t note_id_;
    std::string title_;
    std::string body_;
};

NoteRequest ParseNoteRequest(const userver::server::http::HttpRequest& request,
                                    userver::server::request::RequestContext& context);
}  // namespace nl::dto