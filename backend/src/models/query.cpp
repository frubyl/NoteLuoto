#include "query.hpp"

namespace nl::models {

userver::formats::json::Value Serialize(
    const Query& query,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;

  builder["query_id"] = query.query_id_;
  builder["query"] = query.query_;
  builder["response"] = query.response_;
  builder["created_at"] = query.created_at_;
  return builder.ExtractValue();
}
} // namespace nl::models




