#pragma once

#include <userver/formats/json.hpp>
#include <cstdint>
#include <chrono>
#include <string>
#include <userver/server/http/http_request.hpp>

namespace nl::dto {

struct UserAuthRequest final {
    std::string username_;
    std::string password_;
};

UserAuthRequest ParseAuthRequest(const userver::server::http::HttpRequest& request);


}  // namespace nl::dto  

