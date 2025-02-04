#include <bcrypt.h>
#include <userver/formats/json/serialize_duration.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/utest/utest.hpp>

namespace nl {
// Юнит-тест генерации хэша

UTEST(bcrypt, generateHash) {
  auto password = "frubasik";
  auto password_hash = bcrypt::generateHash(password);
  auto res = bcrypt::validatePassword(password,  password_hash);
  ASSERT_EQ(res, true);
}


}  // namespace nl