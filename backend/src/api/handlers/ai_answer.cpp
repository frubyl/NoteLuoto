#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/logging/log.hpp>
#include <userver/utils/async.hpp>
#include <userver/engine/sleep.hpp>
#include <filesystem>
#include "ai_answer.hpp"
#include "../../db/sql.hpp"

namespace nl::handlers::api::ai::answer {

        Handler::Handler(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& context)
        : HttpHandlerJsonBase(config, context),
          cluster_(context
            .FindComponent<userver::components::Postgres>(
                "postgres-db-1")
            .GetCluster()),
          langchainClient_(context.FindComponent<grpc::clients::LangchainClient>()){}

    userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context) const  {
        if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
            return{};
        }
        int32_t user_id = context.GetData<int32_t>("user_id");

        auto request_body = userver::formats::json::FromString(request.RequestBody());
        auto query =  request_body["question"].As<std::string>();
        if (query.size() < 1 || query.size() > 4096) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
            return buildErrorMessage("Question size error: max lenght exeded or empty question");
        }

        auto answer = langchainClient_.GenerateAnswer(query);

        addQueryAndAnswerToHistory(user_id, query, answer);
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
        return buildResponsebody(answer);
        
    }

    void Handler::addQueryAndAnswerToHistory(int32_t& user_id, std::string& query, std::string& answer) const {
        const auto result = cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
            db::sql::kInsertAiHistory.data(), user_id, query, answer);
    }
    

    userver::formats::json::Value Handler::buildResponsebody(std::string& answer) const {
        userver::formats::json::ValueBuilder response_body;
        response_body["answer"] = answer;
        return response_body.ExtractValue();
    }

    userver::formats::json::Value Handler::buildErrorMessage(std::string message) const {
        userver::formats::json::ValueBuilder response_body;
        response_body["message"] = message;
        return response_body.ExtractValue();
      } 
}  // namespace nl::handlers::api::ai::answer