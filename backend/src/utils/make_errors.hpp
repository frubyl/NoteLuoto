#pragma once

#include <userver/formats/json.hpp>
#include <string>
#include <map> 

namespace nl::utils {
// Создание массива ошибок в формате JSON
userver::formats::json::Value MakeErrors(std::map<std::string, std::string>& errors);

}  // namespace noteluoto::utils