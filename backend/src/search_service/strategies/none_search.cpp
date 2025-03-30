#include "../search_strategy.hpp"
#include <vector>

namespace nl::search_service {
    class NoneSearch : public SearchStrategy {
        public:
            using SearchStrategy::SearchStrategy;

        protected: 
            std::vector<int64_t> searchNotes(const int64_t user_id, const std::string& query) override {
                const auto result =  
                    cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                    db::sql::kGetAllUserNotes.data(), user_id);  

                std::vector<int64_t> notes_ids;
            
                for (auto& row : result) {
                    notes_ids.push_back(row["id"].As<int64_t>());
                }
                return notes_ids;
            }
        }; 
    }