#pragma once
#include <string>
#include <cstdint>
#include <samples/TagRecommender_client.usrv.pb.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/components/component.hpp>
#include <userver/ugrpc/client/client_factory_component.hpp>


namespace nl::grpc::clients  {
    class TagRecommenderClient final  : public userver::components::LoggableComponentBase {
        public:
            static constexpr std::string_view kName = "tag-recommender-client";

            TagRecommenderClient(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);
           
            std::vector<std::string> RecommendTags(int64_t note_id, std::vector<std::string>& existingTags);

            static userver::yaml_config::Schema GetStaticConfigSchema();
            
        private:
            userver::ugrpc::client::ClientFactory& client_factory_;
            tag_recommender::TagRecommenderClient client_;

        };
}; // namespace nl::grpc::clients