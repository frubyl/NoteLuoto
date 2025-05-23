#include "auth_bearer.hpp"
#include <algorithm>
#include <memory>
#include <userver/storages/secdist/component.hpp>
#include <userver/logging/log.hpp>

namespace nl::handlers::auth {

AuthChecker::AuthCheckResult AuthChecker::CheckAuth(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& request_context) const {
  const auto& auth_value = request.GetHeader("Authorization");
  // Проверка на наличие данных в заголовке Authorization
  if (auth_value.empty()) {
    if (is_optional_auth_) {
      return {};
    }
    return AuthCheckResult{
        AuthCheckResult::Status::kTokenNotFound,
        {},
        "Missing authorization token",
        userver::server::handlers::HandlerErrorCode::kUnauthorized};
  }

  // Проверка на наличие токена 
  const auto token_pos = auth_value.find(' ');
  if (token_pos == std::string::npos ||
      std::string_view{auth_value.data(), token_pos} != "Token") {
    return AuthCheckResult{
        AuthCheckResult::Status::kTokenNotFound,
        {},
        "Missing authorization token",
        userver::server::handlers::HandlerErrorCode::kUnauthorized};
  }
  // Декодирование токена 
  const std::string token{auth_value.data() + token_pos + 1};
  jwt::jwt_payload payload;
  try {
    payload = jwt_manager_.DecodeToken(token);
  } catch (...) {
    return AuthCheckResult{
        AuthCheckResult::Status::kInvalidToken,
        {},
        "Invalid token",
        userver::server::handlers::HandlerErrorCode::kUnauthorized};
  }
  // Проверка токена на актуальность 
  const auto token_exp_time_int = payload.get_claim_value<int64_t>("token_exp_time");
  std::chrono::system_clock::time_point token_exp_time = std::chrono::system_clock::from_time_t(token_exp_time_int);

  if (token_exp_time <= std::chrono::system_clock::now()) {
    return AuthCheckResult{
        AuthCheckResult::Status::kInternalCheckFailure,
        {},
        "Token expired",
        userver::server::handlers::HandlerErrorCode::kUnauthorized};
  }
  // Добавление user_id к контексту запроса
  const auto user_id = payload.get_claim_value<int32_t>("user_id");
  request_context.SetData("user_id", user_id);
  return {};
}

userver::server::handlers::auth::AuthCheckerBasePtr CheckerFactory::operator()(
    const userver::components::ComponentContext& context,
    const userver::server::handlers::auth::HandlerAuthConfig& config,
    const userver::server::handlers::auth::AuthCheckerSettings&) const {
  // Получение конфига jwt из конфигурационного файлй
  const auto jwt_config = context.FindComponent<userver::components::Secdist>()
                              .Get()
                              .Get<utils::jwt::JWTConfig>();
  const auto is_optional_auth =
      config.HasMember("optional") ? config["optional"].As<bool>() : false;
  return std::make_shared<AuthChecker>(std::move(jwt_config), is_optional_auth);
 
}

}  // namespace nl::auth