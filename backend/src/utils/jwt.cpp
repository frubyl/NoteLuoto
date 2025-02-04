#include "jwt.hpp"
#include <userver/dynamic_config/storage/component.hpp>
#include <userver/formats/yaml/value_builder.hpp>

namespace nl::utils::jwt {
    JWTConfig::JWTConfig(const userver::formats::json::Value& config) :
          secret_{config["secret"].As<std::string>()},
          token_exp_time_{config["jwt_exp_time"].As<std::chrono::hours>()} {}
    
    JWTManager::JWTManager(const JWTConfig& config) : config_(config){}

    std::string JWTManager::GenerateToken(int32_t user_id) const {
        ::jwt::jwt_object obj{  algorithm("HS256"), 
                                secret(this->config_.secret_),
                            };
        obj.add_claim("user_id", user_id)
           .add_claim("token_exp_time", std::chrono::system_clock::now() + config_.token_exp_time_);

        return obj.signature();
    }

    ::jwt::jwt_payload JWTManager::DecodeToken(std::string_view jwt_token) const {
        auto dec_obj = ::jwt::decode(jwt_token, algorithms({"HS256"}),
                               secret(config_.secret_), verify(true));
        return dec_obj.payload();
    }
}  // namespace nl::utils::jwt