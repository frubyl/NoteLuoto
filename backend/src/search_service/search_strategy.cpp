#include "search_strategy.hpp"
#include <userver/logging/log.hpp>

namespace nl::search_service {

SearchStrategy::SearchStrategy(userver::storages::postgres::ClusterPtr cluster)
    : cluster_(std::move(cluster)) {}

std::vector<models::NoteWithId> SearchStrategy::find(dto::SearchParams& searchParams) {
    std::vector<int64_t> note_ids = searchNotes(searchParams.user_id, searchParams.query);
    return filterByTags(note_ids, searchParams.tags, searchParams.limit, searchParams.page);
}

std::vector<models::NoteWithId> SearchStrategy::filterByTags(
    std::vector<int64_t>& note_ids,
    const std::vector<std::string>& tags,
    int64_t& limit, 
    int64_t& page
) {
    std::vector<models::NoteWithId> notes;
    if (note_ids.empty()) {
        return {};
    }

    int offset = (page - 1)* limit;

    if (tags.empty()) {
        const auto result = cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
            db::sql::kGetNotes.data(),
            note_ids,
            limit, 
            offset
        );
        for (auto& row : result) {
            models::NoteWithId note = row.As<models::NoteWithId>(
                userver::storages::postgres::kRowTag
            );
            notes.push_back(note);
        }
    } else {
        const auto result = cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
            db::sql::kFilterByTags.data(),
            note_ids,
            tags,
            limit, 
            offset
        );

        for (auto& row : result) {
            models::NoteWithId note = row.As<models::NoteWithId>(
                userver::storages::postgres::kRowTag
            );
            LOG_ERROR() << note.note_id_;
            notes.push_back(note);
        }
    }
    return notes;
}

} // namespace nl::search_service