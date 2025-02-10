#include "checklist.hpp"

namespace nl::models {

userver::formats::json::Value Serialize(
    const Item& item,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;

  builder["item_id"] = item.item_id_;
  builder["text"] = item.text_;
  builder["completed"] = item.completed_;
  builder["created_at"] = item.created_at_;
  builder["updated_at"] = item.updated_at_;
  return builder.ExtractValue();
}

userver::formats::json::Value Serialize(
    const Checklist& checklist,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["title"] = checklist.title_;
  builder["created_at"] = checklist.created_at_;
  builder["updated_at"] = checklist.updated_at_;
  return builder.ExtractValue();
}
} // namespace nl::models

