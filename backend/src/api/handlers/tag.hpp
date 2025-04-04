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

namespace nl::handlers::api::tag {

namespace all::get {
class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
 public:
  static constexpr std::string_view kName{"handler-get-tags-all"};

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
} // namespace all::get 


namespace create::post {
    class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
     public:
      static constexpr std::string_view kName{"handler-post-tags-create"};
    
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
} // namespace create::post

namespace note {
namespace post {
    class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
        public:
         static constexpr std::string_view kName{"handler-post-tags-note-tag"};
       
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
} // namespace post 


namespace del {
    class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
     public:
      static constexpr std::string_view kName{"handler-delete-tags-note-tag"};
    
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

namespace get {
    class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
        public:
         static constexpr std::string_view kName{"handler-get-tags-note"};
       
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
} // namespace note

}  // namespace nl::handlers::api::tag
