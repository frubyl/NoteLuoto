#include "../../db/sql.hpp"
#include "tags_create.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

namespace nl::handlers::api::tags::create::post {

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

} // namespace nl::handlers::api::tags::create::post


 