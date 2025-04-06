#include "register.hpp"
#include "../../db/sql.hpp"
#include "../../dto/user.hpp"
#include <bcrypt.h>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <regex>
#include <userver/logging/log.hpp>

namespace nl::handlers::api::reg::post {

  Handler::Handler(const userver::components::ComponentConfig& config,
                  const userver::components::ComponentContext& context)
      : HttpHandlerJsonBase(config, context),
        cluster_(context
                    .FindComponent<userver::components::Postgres>(
                        "postgres-db-1")
                    .GetCluster()){}

  userver::formats::json::Value Handler::HandleRequestJsonThrow(
  const userver::server::http::HttpRequest& request,
  const userver::formats::json::Value& request_json,
  userver::server::request::RequestContext&) const  {
    dto::UserAuthRequest userAuthRequest;
    try {
      userAuthRequest = dto::ParseAuthRequest(request);
    } catch(...) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
      return buildErrorMessage("Invalid json format");
    }

    std::string message = validateUserData(userAuthRequest.username_, userAuthRequest.password_);
    if (message != "") {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
      return buildErrorMessage(message);
    }

    if(findUserInDB(userAuthRequest.username_)) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);  
      return {};
    }

    addNewUserToDB(userAuthRequest.username_, userAuthRequest.password_);
    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);  
    return {};
  }

  bool Handler::findUserInDB(std::string& username) const {
    const auto result = cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                db::sql::kGetUserByUsername.data(), username);
    return !result.IsEmpty();
  }


  void Handler::addNewUserToDB(std::string& username, std::string& password) const {
    auto password_hash = bcrypt::generateHash(password);
    const auto result = cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                  db::sql::kAddNewUser.data(), username, password_hash);
  }

  // возвращает пустую строку если все хорошо, иначе сообщение ошибки 
  std::string Handler::validateUserData(std::string& username, std::string& password) const {
    bool isValidPassword = validatePassword(password);
    bool isValidUsername = validateUsername(username);

    if (!isValidUsername) {
      return "Invalid Username";
    }
    
    if (!isValidPassword) {
      return "Invalid Password";
    }

    return "";

  }

  bool Handler::validatePassword(std::string& password) const {
    std::regex pattern("^(?=.*\\d)(?=.*[a-z]).{8,32}$");
    return std::regex_match(password, pattern);
  }

  bool Handler::validateUsername(std::string& username) const {
    std::regex pattern("^[A-Za-z0-9_-]{3,30}$");
    return std::regex_match(username, pattern);
  }

  userver::formats::json::Value Handler::buildErrorMessage(std::string message) const {
    userver::formats::json::ValueBuilder response_body;
    response_body["message"] = message;
    return response_body.ExtractValue();
  } 

} // namespace nl::handlers::api::reg::post 
