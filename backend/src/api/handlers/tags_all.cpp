#include "../../db/sql.hpp"
#include "../../models/tag.hpp"
#include "tags_all.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

namespace nl::handlers::api::tags::all::get {

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


} // namespace nl::handlers::api::tags::all::get


 