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
        };
} // namespace del
}  // namespace nl::handlers::api::note
