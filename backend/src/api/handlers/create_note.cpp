#include "../../db/sql.hpp"
#include "../../dto/note.hpp"
#include "create_note.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

namespace nl::handlers::api::notes::post {

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

        // Создаем заметку
        const auto result =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kCreateNote.data(), noteRequest.title_, noteRequest.body_, noteRequest.user_id_);

        // Формируем тело ответа 
        userver::formats::json::ValueBuilder response_body;
        const auto result_set = result.AsSetOf<int32_t>();
        int32_t note_id = *(result_set.begin());
        response_body["note_id"] = note_id;  
        request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);  
        return response_body.ExtractValue();
}       
} // namespace nl::handlers::api::notes::post


 