#include "login.hpp"
#include "../../dto/user.hpp"
#include "../../models/user.hpp"
#include "../../db/sql.hpp"
#include <bcrypt.h>
#include <regex>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/row_types.hpp>
namespace nl::handlers::api::login::post {

Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context),
      cluster_(context
                   .FindComponent<userver::components::Postgres>(
                       "postgres-db-1")
                   .GetCluster()),
      jwt_manager_(context.FindComponent<userver::components::Secdist>()
                       .Get()
                       .Get<utils::jwt::JWTConfig>()){}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value&,
    userver::server::request::RequestContext&) const  {
  
  dto::UserAuthRequest userAuthRequest;
  try {
    userAuthRequest = dto::ParseAuthRequest(request);
  } catch(...) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
    return buildErrorMessage("Invalid json");
  }
  if (!validateUserData(userAuthRequest.username_, userAuthRequest.password_)) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
    return buildErrorMessage("Data does not match format");
  }
  // Поиск пользователя по username в базе данных
  const auto result =
      cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                        db::sql::kGetUserByUsername.data(), userAuthRequest.username_);

  // Если пользователь с таким username не существует
  if (result.IsEmpty()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
    return {};
  }
  auto user = result.AsSingleRow<models::User>(userver::storages::postgres::kRowTag);

  // Сравниваем пароли
  if (!bcrypt::validatePassword(userAuthRequest.password_,  user.password_hash_)) {   
    request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);  
    return {};
  }
 
  auto token = jwt_manager_.GenerateToken(user.user_id_);
  userver::formats::json::ValueBuilder builder;

  builder["access_token"] = token;
  request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
  return builder.ExtractValue();
}

userver::formats::json::Value Handler::buildErrorMessage(std::string message) const {
  userver::formats::json::ValueBuilder response_body;
  response_body["message"] = message;
  return response_body.ExtractValue();
} 


bool Handler::validateUserData(std::string& username, std::string& password) const {
  bool isValidPassword = validatePassword(password);
  bool isValidUsername = validateUsername(username);
  return isValidPassword && isValidUsername;

}

bool Handler::validatePassword(std::string& password) const {
  std::regex pattern("^(?=.*\\d)(?=.*[a-z]).{8,32}$");
  return std::regex_match(password, pattern);
}

bool Handler::validateUsername(std::string& username) const {
  std::regex pattern("^[A-Za-z0-9_-]{3,30}$");
  return std::regex_match(username, pattern);
}

} // namespace nl::handlers::api::login::post
