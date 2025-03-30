#include "../search_strategy.hpp"

namespace nl::search_service {
    class ExactSearch : public SearchStrategy {
        public:
            using SearchStrategy::SearchStrategy;

        protected: 
            std::vector<int64_t> searchNotes(const int64_t user_id, const std::string& query) override {
                const auto result =  
                    cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                    db::sql::kGetAllUserNotesWithQuery.data(), user_id, query);  

                std::vector<int64_t> notes_ids;
            
                for (auto& row : result) {
                    notes_ids.push_back(row["id"].As<int64_t>());
                }
                return notes_ids;
            }
        }; 
    }