#include "register.hpp"
#include "../../db/sql.hpp"
#include "../../dto/user.hpp"
#include <bcrypt.h>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

namespace nl::handlers::api::reg::post {

Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      cluster_(context
                   .FindComponent<userver::components::Postgres>(
                       "postgres-db-1")
                   .GetCluster()){}

std::string Handler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const  {
  
    auto userAuthRequest = dto::ParseAuthRequest(request);

  // Поиск пользователя по username в базе данных
  const auto result_username =
      cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                        db::sql::kGetUserByUsername.data(), userAuthRequest.username_);

  // Если пользователь с таким username уже существует, устанавливаем код ответа = 409
  if (!result_username.IsEmpty()) {
    request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);  
    return {};
  }
  
  // Генерируем хэш пароля
  auto password_hash = bcrypt::generateHash(userAuthRequest.password_);
  // Добавляем нового пользователя 
  const auto result =
      cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                        db::sql::kAddNewUser.data(), userAuthRequest.username_, password_hash);

  request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);  
  return {};
}
} // namespace nl::handlers::api::reg::post 
