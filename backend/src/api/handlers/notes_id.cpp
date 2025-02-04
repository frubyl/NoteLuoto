#include "../../db/sql.hpp"
#include "../../dto/note.hpp"
#include "../../models/note.hpp"
#include "notes_id.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/logging/log.hpp>

namespace nl::handlers::api::notes::id {

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
        int32_t note_id = std::stoi(request.GetPathArg("note_id"));
        const auto result =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetNote.data(), note_id);

        if (result.IsEmpty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
            return {};
        }

        // Формируем тело ответа 
        auto note = result.AsSingleRow<models::NoteWithoutId>(userver::storages::postgres::kRowTag);

        userver::formats::json::ValueBuilder response_body;
        response_body = note;
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
        return response_body.ExtractValue();
}    
} // namespace get

namespace patch {
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

        auto noteRequest = dto::ParseNoteRequest(request, context);

        const auto result_get =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetNote.data(), noteRequest.note_id_);

        if (result_get.IsEmpty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
            return {};
        }
        
        if (noteRequest.title_ != "") {
            const auto result_update =  
                cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                    db::sql::kUpdateNoteTitle.data(), noteRequest.title_, noteRequest.note_id_);
        }
        if (noteRequest.body_ != "") {
            const auto result_update =  
                cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                    db::sql::kUpdateNoteBody.data(), noteRequest.body_, noteRequest.note_id_);
        }
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
        return {};
}       
} // namespace patch
} // namespace nl::handlers::api::notes::id 


 