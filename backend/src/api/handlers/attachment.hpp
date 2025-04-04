#pragma once
#include <string_view>
#include <cstdint>
#include <optional>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/postgres_fwd.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/engine/task/task_processor_fwd.hpp>
#include <userver/storages/secdist/component.hpp>
#include "../../utils/file_manager.hpp"
namespace nl::handlers::api::attachment {

namespace post {
class Handler final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName{"handler-post-attachment-note"};

  Handler(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context);

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext& context) const override final;

 private:
  const userver::storages::postgres::ClusterPtr cluster_;
  std::string buildErrorMessage(std::string message) const; 

};
}// namespace post

namespace get {
class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName{"handler-get-attachment"};

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
  static constexpr std::string_view kName{"handler-delete-attachment"};

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
} // namespace del


}  // namespace nl::handlers::api::attachment
