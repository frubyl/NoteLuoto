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

namespace nl::handlers::api::notes::id {

namespace get {
class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName{"handler-get-note-id"};

  Handler(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context);

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& request_json,
      userver::server::request::RequestContext& context) const override final;

 private:
  const userver::storages::postgres::ClusterPtr cluster_;
};
} // namespace get


namespace patch {
class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName{"handler-patch-note-id"};

  Handler(const userver::components::ComponentConfig& config,
          const userver::components::ComponentContext& context);

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& request_json,
      userver::server::request::RequestContext& context) const override final;

 private:
  const userver::storages::postgres::ClusterPtr cluster_;
};
} // namespace patch

}  // namespace nl::handlers::api::notes::id
