#include "../utils/jwt.hpp"
#include <userver/formats/json/serialize_duration.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/utest/utest.hpp>


namespace nl {
// Юнит-тест генерации JWT
namespace {

constexpr uint64_t kUserId{12};

userver::formats::json::Value GetTestConfigJson() {
  userver::formats::json::ValueBuilder builder;
  builder["secret"] = "secret";
  builder["jwt_exp_time"] = std::chrono::hours{10};
  return builder.ExtractValue();
}

}  // namespace

UTEST(JWTManager, GenerateToken) {
  const utils::jwt::JWTConfig config{GetTestConfigJson()};
  const utils::jwt::JWTManager jwt_manager{config};
  const auto token = jwt_manager.GenerateToken(kUserId);
  const auto decoded_token = jwt_manager.DecodeToken(token);
  auto id = decoded_token.get_claim_value<int32_t>("user_id");
  ASSERT_EQ(id, kUserId);
}


}  // namespace nl