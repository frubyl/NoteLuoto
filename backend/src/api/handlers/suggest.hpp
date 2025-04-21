#pragma once
#include <string_view>
#include <optional>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/postgres_fwd.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/engine/task/task_processor_fwd.hpp>
#include "../../grpc_clients/grpc_langchain_client.hpp"
#include "../../grpc_clients/grpc_tag_recommender_client.hpp"

namespace nl::handlers::api::suggest {

namespace queries {
        class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
                public:
                static constexpr std::string_view kName{"handler-get-suggest-queries"};

                Handler(const userver::components::ComponentConfig& config,
                        const userver::components::ComponentContext& context);

                userver::formats::json::Value HandleRequestJsonThrow(
                const userver::server::http::HttpRequest& request,
                const userver::formats::json::Value& request_json,
                userver::server::request::RequestContext& context) const override final;

        private:
                nl::grpc::clients::LangchainClient& langchainClient_;
                userver::formats::json::Value buildResponsebody(std::vector<std::string>& suggestions) const;

};
} // namespace queries

namespace tags {
        class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
                public:
                static constexpr std::string_view kName{"handler-post-suggest-tags"};

                Handler(const userver::components::ComponentConfig& config,
                        const userver::components::ComponentContext& context);

                userver::formats::json::Value HandleRequestJsonThrow(
                const userver::server::http::HttpRequest& request,
                const userver::formats::json::Value& request_json,
                userver::server::request::RequestContext& context) const override final;

        private:
                nl::grpc::clients::TagRecommenderClient& tagRecommenderClient_;
                const userver::storages::postgres::ClusterPtr cluster_;

                std::vector<std::string> getAllTags(int64_t user_id) const;
                userver::formats::json::Value buildResponsebody(std::vector<std::string>& suggestions) const;
                userver::formats::json::Value buildErrorMessage(std::string message) const; 

};
} // namespace tags
}  // namespace nl::handlers::api::suggest
