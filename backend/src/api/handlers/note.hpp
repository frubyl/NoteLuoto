#pragma once
#include <string_view>
#include <optional>
#include "../../dto/note.hpp"
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/postgres_fwd.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/engine/task/task_processor_fwd.hpp>
#include "../../grpc_clients/grpc_sync_client.hpp"
#include "../../utils/data_to_text_formatter.hpp"

namespace nl::handlers::api::note {

namespace get {
class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName{"handler-get-note"};

  Handler(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context);

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& request_json,
      userver::server::request::RequestContext& context) const override final;

 private:
  const userver::storages::postgres::ClusterPtr cluster_;
  userver::formats::json::Value buildErrorMessage(std::string message) const;

};
} // namespace get


namespace patch {
class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName{"handler-patch-note"};

  Handler(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context);

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& request_json,
      userver::server::request::RequestContext& context) const override final;

 private:
  const userver::storages::postgres::ClusterPtr cluster_;
  void updateNote(dto::NoteRequest& noteRequest) const;
grpc::clients::NoteSyncClient& client_; 
  utils::DataToTextFormatter dataToTextFormatter_;
  userver::formats::json::Value buildErrorMessage(std::string message) const;

};
} // namespace patch

namespace post {
class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
        public:
         static constexpr std::string_view kName{"handler-post-note"};
       
         Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context);
       
         userver::formats::json::Value HandleRequestJsonThrow(
             const userver::server::http::HttpRequest& request,
             const userver::formats::json::Value& request_json,
             userver::server::request::RequestContext& context) const override final;
       
        private:
         const userver::storages::postgres::ClusterPtr cluster_;
           grpc::clients::NoteSyncClient& client_; 
         utils::DataToTextFormatter dataToTextFormatter_;
         userver::formats::json::Value buildErrorMessage(std::string message) const;

        };
} // namespace post

namespace del {
class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
        public:
                static constexpr std::string_view kName{"handler-delete-note"};
        
                Handler(const userver::components::ComponentConfig& config,
                        const userver::components::ComponentContext& context);
        
                userver::formats::json::Value HandleRequestJsonThrow(
                const userver::server::http::HttpRequest& request,
                const userver::formats::json::Value& request_json,
                userver::server::request::RequestContext& context) const override final;
        
        private:
                const userver::storages::postgres::ClusterPtr cluster_;
                grpc::clients::NoteSyncClient& client_; 
                userver::formats::json::Value buildErrorMessage(std::string message) const;

        };
} // namespace del
}  // namespace nl::handlers::api::note
