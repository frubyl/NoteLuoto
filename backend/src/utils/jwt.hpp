#pragma once

#include <jwt/jwt.hpp>
#include <string>
#include <chrono>
#include <userver/formats/json/value.hpp>
namespace nl::utils::jwt {

using namespace::jwt::params;

// Конфиг jwt - подгружается из файла
struct JWTConfig final {
  JWTConfig(const userver::formats::json::Value& config);

  const std::string secret_;
  const std::chrono::hours token_exp_time_;
};

// Класс для работы с токенами
class JWTManager final {
 public:
  JWTManager(const JWTConfig& config);
  
  std::string GenerateToken(int32_t user_id) const;
  ::jwt::jwt_payload DecodeToken(std::string_view jwt_token) const;

 private:
  const JWTConfig config_;
};

}  // namespace nl::utils::jwt