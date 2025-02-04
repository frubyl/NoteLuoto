#include "note.hpp"

namespace nl::models {

userver::formats::json::Value Serialize(
    const NoteWithId& noteWithId,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;

  builder["note_id"] = noteWithId.note_id_;
  builder["title"] = noteWithId.title_;
  builder["body"] = noteWithId.body_;
  builder["created_at"] = noteWithId.created_at_;
  builder["updated_at"] = noteWithId.updated_at_;
  return builder.ExtractValue();
}

userver::formats::json::Value Serialize(
    const NoteWithoutId& noteWithoutId,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;

  builder["title"] = noteWithoutId.title_;
  builder["body"] = noteWithoutId.body_;
  builder["created_at"] = noteWithoutId.created_at_;
  builder["updated_at"] = noteWithoutId.updated_at_;
  return builder.ExtractValue();
}
} // namespace nl::models


