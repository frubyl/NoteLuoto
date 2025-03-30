#pragma once

#include <string>
#include <vector>
#include <optional>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/postgres_fwd.hpp>
#include "../models/note.hpp"
#include "../db/sql.hpp"
#include "../dto/search_params.hpp"

namespace nl::search_service {

class SearchStrategy {
public:
    SearchStrategy(userver::storages::postgres::ClusterPtr cluster);
    virtual ~SearchStrategy() = default;

    std::vector<models::NoteWithId> find(dto::SearchParams&);

protected:
    const userver::storages::postgres::ClusterPtr cluster_;

    virtual std::vector<int64_t> searchNotes(const int64_t user_id, const std::string& query) = 0;

    std::vector<models::NoteWithId> filterByTags(
        std::vector<int64_t>& note_ids,
        const std::vector<std::string>& tags,
        int64_t& limit, 
        int64_t& page
    );
};

} // namespace nl::search_service

