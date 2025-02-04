#include "../../db/sql.hpp"
#include "../../models/tag.hpp"
#include "tags_note_tag.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

namespace nl::handlers::api::tags::note::tag  {

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

} // namespace nl::handlers::api::tags::note::tag 


 