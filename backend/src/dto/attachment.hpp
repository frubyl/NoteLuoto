#pragma once

#include <userver/formats/json.hpp>
#include <cstdint>
#include <chrono>
#include <string>
#include <userver/server/http/http_request.hpp>
#include <userver/server/http/form_data_arg.hpp>
namespace nl::dto {

struct AttachmentRequest final {
    int32_t note_id_;
    int32_t attachment_id_;
    userver::server::http::FormDataArg file_;
};

AttachmentRequest ParseAttachmentRequest(const userver::server::http::HttpRequest& request);

}  // namespace nl::dto  

