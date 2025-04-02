#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/logging/log.hpp>
#include <userver/utils/async.hpp>
#include <userver/engine/sleep.hpp>
#include <filesystem>
#include "suggest.hpp"
#include "../../db/sql.hpp"

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

                auto note_id = std::stoi(request.GetPathArg("note_id"));
                auto existingTags = getNoteTags(note_id);

                auto suggestions = tagRecommenderClient_.RecommendTags(note_id, existingTags);
                request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
                return buildResponsebody(suggestions);
                
  
                
            }

            std::vector<std::string> Handler::getNoteTags(int64_t note_id) const {
                const auto result = cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                    db::sql::kGetNoteTagsWithoutId.data(), note_id);

                std::vector<std::string> existingTags;
                for (auto& row : result) {
                    std::string tag = row.As<std::string>();
                    existingTags.push_back(tag);
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
    } // namespace tags
}  // namespace nl::handlers::api::suggest