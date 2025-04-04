#pragma once

#include <userver/formats/json.hpp>
#include <cstdint>
#include <chrono>
#include <string>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
namespace nl::dto {

struct HistoryRequest final {
    int32_t user_id_;
    int32_t page_ = 1;
    int32_t limit_ = 20;

};

HistoryRequest ParseHistoryRequest(const userver::server::http::HttpRequest& request,
                                    userver::server::request::RequestContext& context);
}  // namespace nl::dto  

