#include "../dto/search_params.hpp"
#include <userver/storages/postgres/postgres_fwd.hpp>
#include "strategies/none_search.cpp"
#include "strategies/exact_search.cpp"
#include "strategies/semantic_search.cpp"

namespace nl::search_service {

    class SearchService {
        public:
            SearchService(const userver::storages::postgres::ClusterPtr cluster) : cluster_(cluster) {}

            std::unique_ptr<SearchStrategy> CreateStrategy(dto::SearchType& searchType, nl::grpc::clients::LangchainClient& langchainClient ) const {
                switch (searchType) {
                    case dto::SearchType::EXACT:
                        return std::make_unique<ExactSearch>(cluster_);
                    case dto::SearchType::NONE: 
                        return std::make_unique<NoneSearch>(cluster_);
                    case dto::SearchType::SEMANTIC: 
                        return std::make_unique<SemanticSearch>(cluster_, langchainClient);
                }
            }


        protected: 
            const userver::storages::postgres::ClusterPtr cluster_;     
    };
}