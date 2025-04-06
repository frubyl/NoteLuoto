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
   if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
      return{};
  }
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
   userver::formats::json::Value Handler::buildErrorMessage(std::string message) const {
      userver::formats::json::ValueBuilder response_body;
      response_body["message"] = message;
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
   if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
      return{};
  }
   auto user_id = context.GetData<int32_t>("user_id");
   userver::formats::json::Value request_body;
   std::string tag_name;
   try {
      request_body = userver::formats::json::FromString(request.RequestBody());
      tag_name = request_body["name"].As<std::string>();
   } catch(...) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
      return buildErrorMessage("Invalid request parametrs");
   }
   if (tag_name.size() > 50 || tag_name.size() < 1) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
      return buildErrorMessage("Tag size error: max lenght exeded or lenght less 1");
   }
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
userver::formats::json::Value Handler::buildErrorMessage(std::string message) const {
   userver::formats::json::ValueBuilder response_body;
   response_body["message"] = message;
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
   if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
      return{};
  }
   if (!request.HasPathArg("note_id")) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
      return buildErrorMessage("Note_id in path is empty");
   }
   auto note_id = std::stoi(request.GetPathArg("note_id"));
   if (note_id < 1) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
      return buildErrorMessage("Note_id less than 1");
   }
   if (!request.HasPathArg("tag_id")) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
      return buildErrorMessage("tag_id in path is empty");
   }
   auto tag_id = std::stoi(request.GetPathArg("tag_id"));
   if (tag_id < 1) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
      return buildErrorMessage("tag_id less than 1");
   }

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
userver::formats::json::Value Handler::buildErrorMessage(std::string message) const {
   userver::formats::json::ValueBuilder response_body;
   response_body["message"] = message;
   return response_body.ExtractValue();
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
   if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
      return{};
  }
   if (!request.HasPathArg("note_id")) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
      return buildErrorMessage("Note_id in path is empty");
   }
   auto note_id = std::stoi(request.GetPathArg("note_id"));
   if (note_id < 1) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
      return buildErrorMessage("Note_id less than 1");
   }
   if (!request.HasPathArg("tag_id")) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
      return buildErrorMessage("tag_id in path is empty");
   }
   auto tag_id = std::stoi(request.GetPathArg("tag_id"));
   if (tag_id < 1) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
      return buildErrorMessage("tag_id less than 1");
   }
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
   userver::formats::json::Value Handler::buildErrorMessage(std::string message) const {
      userver::formats::json::ValueBuilder response_body;
      response_body["message"] = message;
      return response_body.ExtractValue();
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
   if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
      return{};
  }
   if (!request.HasPathArg("note_id")) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
      return buildErrorMessage("Note_id in path is empty");
   }
   auto note_id = std::stoi(request.GetPathArg("note_id"));
   if (note_id < 1) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
      return buildErrorMessage("Note_id less than 1");
   }
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
userver::formats::json::Value Handler::buildErrorMessage(std::string message) const {
   userver::formats::json::ValueBuilder response_body;
   response_body["message"] = message;
   return response_body.ExtractValue();
 } 
} // namespace get
} // namespace note

}  // namespace nl::handlers::api::tag
