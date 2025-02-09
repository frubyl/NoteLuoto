#include "user.hpp"

namespace nl::dto {
    
UserAuthRequest ParseAuthRequest(const userver::server::http::HttpRequest& request) {
    UserAuthRequest userAuthRequest;
    auto request_body = userver::formats::json::FromString(request.RequestBody());
    userAuthRequest.username_ = request_body["username"].As<std::string>();
    userAuthRequest.password_ = request_body["password"].As<std::string>();
    return userAuthRequest;

}

}  // namespace nl::dto  