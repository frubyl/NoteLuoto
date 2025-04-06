#include "../../models/query.hpp"
#include "../../dto/history.hpp"
#include "../../db/sql.hpp"
#include "history.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/row_types.hpp>
#include <userver/logging/log.hpp>

namespace nl::handlers::api::history::get {

Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context),
      cluster_(context
                   .FindComponent<userver::components::Postgres>(
                       "postgres-db-1")
                   .GetCluster()){}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value&,
    userver::server::request::RequestContext& context) const  {
        if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
            return{};
        }
        dto::HistoryRequest historyRequest;

        try {
            historyRequest = dto::ParseHistoryRequest(request, context);
        } catch (std::exception& ex) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest); 
            return buildErrorMessage(ex.what());

        }
        auto offset = (historyRequest.page_- 1) * historyRequest.limit_;

        // Получаем записи с пагинацией 
        const auto result =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetUserHistory.data(), historyRequest.user_id_, historyRequest.limit_, offset);
        LOG_ERROR() << result.IsEmpty();
        // Формируем тело ответа 
        userver::formats::json::ValueBuilder response_body;
        response_body["history"] = userver::formats::common::Type::kArray;  

        for (const auto& row : result) {
            response_body["history"].PushBack(row.As<models::Query>(
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
} // namespace nl::handlers::api::history::get


 