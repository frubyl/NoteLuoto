#include "../../db/sql.hpp"
#include "../../dto/item.hpp"
#include "checklist_item.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

namespace nl::handlers::api::checklist::item {

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
        auto item = dto::ParseItemRequest(request);

        const auto result_find =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetChecklist.data(), item.checklist_id_);

        if (result_find.IsEmpty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
            return {};
        }

        const auto result_create =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kCreateChecklistItem.data(), item.checklist_id_, item.text_.value());

        userver::formats::json::ValueBuilder response_body;
        const auto result_set = result_create.AsSetOf<int32_t>();
        int32_t item_id = *(result_set.begin());
        response_body["item_id"] = item_id;  
        request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);  
        return response_body.ExtractValue();
}    
} // namespace post

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
        auto item = dto::ParseItemRequest(request);

        const auto result_find =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetChecklistItem.data(), item.item_id_);

        if (result_find.IsEmpty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
            return {};
        }

        if (item.text_) {
            const auto result = 
                cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                    db::sql::kUpdateCheckListItemText.data(), item.text_.value(), item.item_id_);
        }

        if (item.status_) {
            const auto result = 
                cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                    db::sql::kUpdateCheckListItemStatus.data(), item.status_.value(), item.item_id_);
        }
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
        return {};
}    
} // namespace patch

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
        auto item = dto::ParseItemRequest(request);

        const auto result_find =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetChecklistItem.data(), item.item_id_);

        if (result_find.IsEmpty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
            return {};
        }
        const auto result =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kDeleteChecklistItem.data(), item.item_id_);

        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
        return {};
}    
} // namespace del

} // namespace nl::handlers::api::checklist::item

 