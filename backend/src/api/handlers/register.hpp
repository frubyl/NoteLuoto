#pragma once
#include <string_view>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/postgres_fwd.hpp>

namespace nl::handlers::api::reg::post {

class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
public:
  static constexpr std::string_view kName{"handler-post-api-register"};

        Handler(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& context);

        userver::formats::json::Value HandleRequestJsonThrow(
                const userver::server::http::HttpRequest& request,
                const userver::formats::json::Value& request_json,
                userver::server::request::RequestContext& context) const override final;

private:
        const userver::storages::postgres::ClusterPtr cluster_;
        bool validatePassword(std::string& password) const;
        bool validateUsername(std::string& username) const;
        void addNewUserToDB(std::string& username, std::string& password) const;
        bool findUserInDB(std::string& username) const;
        // возвращает пустую строку если все хорошо, иначе сообщение ошибки 
        std::string validateUserData(std::string& username, std::string& password) const;
        userver::formats::json::Value buildErrorMessage(std::string message) const; 

        
};


}  // namespace nl::handlers::api::reg::post