#pragma once
#include <userver/server/handlers/auth/digest/auth_checker_settings_component.hpp>
#include <userver/server/handlers/auth/auth_checker_factory.hpp>
#include <userver/server/handlers/auth/auth_checker_settings.hpp>
#include <userver/server/handlers/auth/handler_auth_config.hpp>

#include "../../utils/jwt.hpp"

namespace nl::handlers::auth {
// Ручка для проверки JWT токена
class AuthChecker final
    : public userver::server::handlers::auth::AuthCheckerBase {
 public:
  using AuthCheckResult = userver::server::handlers::auth::AuthCheckResult;

  AuthChecker(const utils::jwt::JWTConfig& config)
      : jwt_manager_{config}{}

// Основная проверка
  [[nodiscard]] AuthCheckResult CheckAuth(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext& request_context) const override;

  [[nodiscard]] bool SupportsUserAuth() const noexcept override { return true; }

 private:
  const utils::jwt::JWTManager jwt_manager_;

};

class CheckerFactory final : public userver::server::handlers::auth::AuthCheckerFactoryBase {
    public:
        static constexpr std::string_view kAuthType = "bearer";
    
        explicit CheckerFactory(const userver::components::ComponentContext& context);
    
        userver::server::handlers::auth::AuthCheckerBasePtr MakeAuthChecker(
            const userver::server::handlers::auth::HandlerAuthConfig& auth_config
        ) const override;
    private: 
        const utils::jwt::JWTConfig jwt_config_;
    };

}  // namespace nl::handlers::auth