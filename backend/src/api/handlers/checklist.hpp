#pragma once
#include <string_view>
#include <cstdint>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/postgres_fwd.hpp>
#include <userver/storages/secdist/component.hpp>
#include "../../utils/data_to_text_formatter.hpp"
#include "../../grpc_clients/grpc_sync_client.hpp"

namespace nl::handlers::api::checklist {

namespace note::post {
class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName{"handler-post-checklist-note"};

  Handler(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context);

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& request_json,
      userver::server::request::RequestContext& context) const override final;

 private:
  const userver::storages::postgres::ClusterPtr cluster_;
  utils::DataToTextFormatter dataToTextFormatter_;
  grpc::clients::NoteSyncClient& client_; 
  userver::formats::json::Value buildErrorMessage(std::string message) const;

};
}// namespace note::post

namespace get {
class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName{"handler-get-checklist"};

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

namespace del {
class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName{"handler-delete-checklist"};

  Handler(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context);

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& request_json,
      userver::server::request::RequestContext& context) const override final;

 private:
  const userver::storages::postgres::ClusterPtr cluster_;
  utils::DataToTextFormatter dataToTextFormatter_;
  grpc::clients::NoteSyncClient& client_; 
  userver::formats::json::Value buildErrorMessage(std::string message) const;


  int32_t getNoteId(int32_t checklist_id) const;


};
} // namespace del


namespace patch {
class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName{"handler-patch-checklist"};

  Handler(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context);

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& request_json,
      userver::server::request::RequestContext& context) const override final;

 private:
  const userver::storages::postgres::ClusterPtr cluster_;
  utils::DataToTextFormatter dataToTextFormatter_;
  grpc::clients::NoteSyncClient& client_; 

  int32_t getNoteId(int32_t checklist_id) const;
  userver::formats::json::Value buildErrorMessage(std::string message) const;



};
} // namespace patch

}  // namespace nl::handlers::api::checklist
