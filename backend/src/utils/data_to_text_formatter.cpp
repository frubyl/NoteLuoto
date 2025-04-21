#include "data_to_text_formatter.hpp"
#include <sstream> 
#include "../models/note.hpp"
#include "../models/checklist.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include "../db/sql.hpp"

#include <iomanip>
#include <ctime>

namespace nl::utils {
    DataToTextFormatter::DataToTextFormatter(userver::storages::postgres::ClusterPtr cluster) : cluster_ (cluster) {}


    std::string DataToTextFormatter::FormatNote(const int32_t noteId) const {
        const auto result = cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
            db::sql::kGetNote.data(), noteId);
        
        auto note = result.AsSingleRow<models::NoteWithoutId>(userver::storages::postgres::kRowTag);

        auto tt = std::chrono::system_clock::to_time_t(note.updated_at_);
        std::tm tm = *std::gmtime(&tt);

        std::ostringstream ss;

        ss << "updated_at:\t" << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ") << '\n' 
        << "title:\t" << note.title_ << '\n'
        << "text:\t" << note.body_ << '\n';

        std::string formatChecklists = FormatChecklists(noteId);
        return ss.str() + formatChecklists;
        
    }


    std::string DataToTextFormatter::FormatChecklists(const int32_t noteId) const {
       std::ostringstream ss;
       ss << "checklists: \n"; 
       auto result =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetAllNoteChecklists.data(), noteId);
       if (result.IsEmpty()) {
            return ss.str();
       }
       for (auto row : result) {
            auto checklist = row.As<models::ChecklistWithId>(userver::storages::postgres::kRowTag);
            ss << checklist.title_ << '\n';
            ss << FormatItem(checklist.checklist_id_);
            ss << '\n';
       }
       return ss.str();
    }

    std::string DataToTextFormatter::FormatItem(int32_t checklistId) const {
        auto result =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                            db::sql::kGetAllChecklistItems.data(), checklistId);
        std::ostringstream ss;
            
        for (auto row : result) {
            auto item = row.As<models::Item>(userver::storages::postgres::kRowTag);
            ss << "- " << item.text_ << (item.completed_ ? "DONE" : "TODO") << '\n';
        }
        return ss.str();
    }

    
} // nl::utils 