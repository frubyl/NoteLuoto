#include "tag.hpp"

namespace nl::models {

userver::formats::json::Value Serialize(
    const Tag& tag,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;

  builder["tag_id"] = tag.tag_id_;
  builder["name"] = tag.name_;
  return builder.ExtractValue();
}

} // namespace nl::models


