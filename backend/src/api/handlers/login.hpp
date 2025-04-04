#pragma once
#include <string_view>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/postgres_fwd.hpp>
#include <userver/storages/secdist/component.hpp>
#include "../../utils/jwt.hpp"

namespace nl::handlers::api::login::post {

class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName{"handler-post-api-login"};

  Handler(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context);

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& request_json,
      userver::server::request::RequestContext& context) const override final;

 private:
  const userver::storages::postgres::ClusterPtr cluster_;
  const utils::jwt::JWTManager jwt_manager_;
  userver::formats::json::Value buildErrorMessage(std::string message) const; 
  bool validatePassword(std::string& password) const;
  bool validateUsername(std::string& username) const;
  bool validateUserData(std::string& username, std::string& password) const;

};


}  // namespace nl::handlers::api::login::post 
