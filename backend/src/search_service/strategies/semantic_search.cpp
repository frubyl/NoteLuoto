#include "../../grpc_clients/grpc_langchain_client.hpp"
#include "../search_strategy.hpp"
#include <userver/logging/log.hpp>

namespace nl::search_service {
    class SemanticSearch : public SearchStrategy {
        public:
            SemanticSearch(userver::storages::postgres::ClusterPtr cluster, nl::grpc::clients::LangchainClient& langchainClient) :
            SearchStrategy(cluster), langchainClient_(langchainClient) {}
        private: 
            nl::grpc::clients::LangchainClient& langchainClient_;

        protected: 
            std::vector<int64_t> searchNotes(const int64_t user_id, const std::string& query) override {
                std::vector<int64_t> all_notes_ids = langchainClient_.SemanticSearch(query);
                const auto result =  
                cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kFilterByUser.data(), all_notes_ids, user_id);  
                std::vector<int64_t> notes_ids;
                for (auto& row : result) {
                    notes_ids.push_back(row["id"].As<int64_t>());
                }
                return notes_ids;
            }
        }; 
}