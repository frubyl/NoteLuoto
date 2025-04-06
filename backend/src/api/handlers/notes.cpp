#include "../../dto/search_params.hpp"
#include "notes.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/logging/log.hpp>
#include <userver/utils/async.hpp>
#include <userver/engine/sleep.hpp>
#include <filesystem>

namespace nl::handlers::api::get::notes{

    Handler::Handler(const userver::components::ComponentConfig& config,
                        const userver::components::ComponentContext& context)
        : HttpHandlerJsonBase(config, context),
            cluster_(context
                    .FindComponent<userver::components::Postgres>("postgres-db-1")
                    .GetCluster()),
            langchainClient_(context.FindComponent<grpc::clients::LangchainClient>()),
            searchService_(cluster_){}

        userver::formats::json::Value Handler::HandleRequestJsonThrow( 
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& request_json,
        userver::server::request::RequestContext& context) const  {
        if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
            return{};
        }
        dto::SearchParams searchParams;
        try {
            searchParams = dto::ParseSearchParams(request, context);
        } catch(std::exception& ex) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
            return buildErrorMessage(ex.what());
        }
        auto strategy = searchService_.CreateStrategy(searchParams.searchType, langchainClient_);
        auto findNotes = strategy->find(searchParams);

        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
        return buildResponsebody(findNotes);
       
    }  
    
    userver::formats::json::Value Handler::buildResponsebody(std::vector<models::NoteWithId>& findNotes) const {
        userver::formats::json::ValueBuilder response_body;
        response_body["total_count"] = findNotes.size();
        response_body["notes"] = userver::formats::common::Type::kArray;  

        for (const auto& note : findNotes) {
            response_body["notes"].PushBack(note);
        }
        return response_body.ExtractValue();

       
    }
    userver::formats::json::Value Handler::buildErrorMessage(std::string message) const {
        userver::formats::json::ValueBuilder response_body;
        response_body["message"] = message;
        return response_body.ExtractValue();
      }   
    
} // namespace nl::handlers::api::get::notes


 