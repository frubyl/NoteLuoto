#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/logging/log.hpp>
#include <userver/utils/async.hpp>
#include <userver/engine/sleep.hpp>
#include <filesystem>
#include "suggest.hpp"
#include "../../db/sql.hpp"
#include "../../models/tag.hpp"

namespace nl::handlers::api::suggest {

    namespace queries {
        Handler::Handler(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& context)
        : HttpHandlerJsonBase(config, context),
          langchainClient_(context.FindComponent<grpc::clients::LangchainClient>()){}

    userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context) const  {
        if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
            return{};
        }
        auto suggestions = langchainClient_.RecommendPrompts();
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
        return buildResponsebody(suggestions);
     
        
    }

    userver::formats::json::Value Handler::buildResponsebody(std::vector<std::string>& suggestions) const {
        userver::formats::json::ValueBuilder response_body = userver::formats::common::Type::kArray;  

        for (const auto& suggest : suggestions) {
            response_body.PushBack(suggest);
        }
        return response_body.ExtractValue();
    }

    } // namespace queries
    
    namespace tags {
        Handler::Handler(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& context)
        : HttpHandlerJsonBase(config, context),
          cluster_(context
              .FindComponent<userver::components::Postgres>(
                  "postgres-db-1")
              .GetCluster()),
          tagRecommenderClient_(context.FindComponent<grpc::clients::TagRecommenderClient>()){}

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
                auto user_id = context.GetData<int32_t>("user_id");
                auto existingTags = getAllTags(user_id);

                auto suggestions = tagRecommenderClient_.RecommendTags(note_id, existingTags);
                request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
                return buildResponsebody(suggestions);
                
  
                
            }

            std::vector<std::string> Handler::getAllTags(int64_t user_id) const {
                const auto result = cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                    db::sql::kGetAllTags.data(), user_id);

                std::vector<std::string> existingTags;
                for (auto& row : result) {
                    auto tag = row.As<models::Tag>(userver::storages::postgres::kRowTag);
                    existingTags.push_back(tag.name_);
                }   

                return existingTags;
            }

            userver::formats::json::Value Handler::buildResponsebody(std::vector<std::string>& suggestions) const {
                userver::formats::json::ValueBuilder response_body = userver::formats::common::Type::kArray;  
        
                for (const auto& suggest : suggestions) {
                    response_body.PushBack(suggest);
                }
                return response_body.ExtractValue();
            }
            userver::formats::json::Value Handler::buildErrorMessage(std::string message) const {
                userver::formats::json::ValueBuilder response_body;
                response_body["message"] = message;
                return response_body.ExtractValue();
              }   
    } // namespace tags
}  // namespace nl::handlers::api::suggest