#include "../../db/sql.hpp"
#include "../../models/tag.hpp"
#include "tag.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

namespace nl::handlers::api::tag {

namespace all::get {
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
userver::server::request::RequestContext& context) const  {
auto user_id = context.GetData<int32_t>("user_id");

const auto result =
   cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                       db::sql::kGetAllTags.data(), user_id);

userver::formats::json::ValueBuilder response_body = userver::formats::common::Type::kArray;
for (const auto& row : result) {
   response_body.PushBack(row.As<models::Tag>(
   userver::storages::postgres::kRowTag));
}

request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
return response_body.ExtractValue();
}
} // namespace all::get 


namespace create::post {

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
userver::server::request::RequestContext& context) const  {
auto user_id = context.GetData<int32_t>("user_id");

auto request_body = userver::formats::json::FromString(request.RequestBody());
auto tag_name = request_body["name"].As<std::string>();

const auto result_find =
   cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                       db::sql::kFindTagByName.data(), tag_name, user_id);

if (!result_find.IsEmpty()) {
   request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);  
   return {};
}

const auto result_create =
   cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                       db::sql::kCreateTag.data(), tag_name, user_id);

// Формируем тело ответа 
const auto result_set = result_create.AsSetOf<int32_t>();
int32_t tag_id = *(result_set.begin());

userver::formats::json::ValueBuilder response_body;
response_body["tag_id"] = tag_id;  
request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);  
return response_body.ExtractValue();
}    
} // namespace create::post

namespace note {
namespace post {
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
userver::server::request::RequestContext& context) const  {
auto note_id = std::stoi(request.GetPathArg("note_id"));
auto tag_id = std::stoi(request.GetPathArg("tag_id"));

const auto result_find_note =
   cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                       db::sql::kGetNote.data(), note_id);

if(result_find_note.IsEmpty()) {
   request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
   return {};
}

const auto result_find_tag =
   cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                       db::sql::kGetTagById.data(), tag_id);

if(result_find_tag.IsEmpty()) {
   request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
   return {};
}

const auto result_find_relationship =
   cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                       db::sql::kCheckTagNote.data(), note_id, tag_id);

if (!result_find_relationship.IsEmpty()) {
   request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);  
   return {};  
}

const auto result =
   cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                       db::sql::kAddTagToNote.data(), note_id, tag_id);

request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
return {};
}    
} // namespace post 


namespace del {
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
userver::server::request::RequestContext& context) const  {
auto note_id = std::stoi(request.GetPathArg("note_id"));
auto tag_id = std::stoi(request.GetPathArg("tag_id"));

const auto result_find_note =
   cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                       db::sql::kGetNote.data(), note_id);

if(result_find_note.IsEmpty()) {
   request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
   return {};
}

const auto result_find_tag =
   cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                       db::sql::kGetTagById.data(), tag_id);

if(result_find_tag.IsEmpty()) {
   request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
   return {};
}

const auto result_find_relationship =
   cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                       db::sql::kCheckTagNote.data(), note_id, tag_id);

if (result_find_relationship.IsEmpty()) {
   request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
   return {};  
}

const auto result =
   cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                       db::sql::kDeleteTagFromNote.data(), note_id, tag_id);

request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
return {};
}    
} // namespace del

namespace get {
   
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
userver::server::request::RequestContext& context) const  {
auto note_id = std::stoi(request.GetPathArg("note_id"));

const auto result_find =
cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                   db::sql::kGetNote.data(), note_id);

if(result_find.IsEmpty()) {
request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
return {};
}
const auto result =
cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                   db::sql::kGetNoteTags.data(), note_id);

userver::formats::json::ValueBuilder response_body = userver::formats::common::Type::kArray;
for (const auto& row : result) {
response_body.PushBack(row.As<models::Tag>(
userver::storages::postgres::kRowTag));
}

request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
return response_body.ExtractValue();
} 
} // namespace get
} // namespace note

}  // namespace nl::handlers::api::tag
