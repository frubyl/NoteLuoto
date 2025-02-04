#include "make_errors.hpp"

namespace nl::utils {

userver::formats::json::Value MakeErrors(std::map<std::string, std::string>& errors) {
    userver::formats::json::ValueBuilder errors_builder =
      userver::formats::json::MakeArray();
    // Добавляем ошибки в JSON
    for (auto el : errors) {
        userver::formats::json::ValueBuilder error_builder;
        error_builder[el.first] = el.second;
        errors_builder.PushBack(error_builder.ExtractValue());
    }
    return errors_builder.ExtractValue();
}

}  // namespace noteluoto::utils