#pragma once
#include <string> 
#include <chrono> 
#include <cstdint> 
#include "../models/note.hpp"
#include "../db/sql.hpp"
#include "../models/checklist.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>

namespace nl::utils {
    class DataToTextFormatter {
        public:
            DataToTextFormatter(userver::storages::postgres::ClusterPtr cluster);

            std::string FormatNote(const int32_t noteId) const;

        private:
            std::string FormatItem(int32_t checklistId) const;
            std::string FormatChecklists(const int32_t noteId) const;
            const userver::storages::postgres::ClusterPtr cluster_;

    };
} // nl::utils 