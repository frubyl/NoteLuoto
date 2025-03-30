#include "grpc_tag_recommender_client.hpp"
#include <userver/clients/dns/component.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

namespace nl::grpc::clients {
    userver::yaml_config::Schema TagRecommenderClient::GetStaticConfigSchema() {
        return userver::yaml_config::MergeSchemas<
            userver::components::LoggableComponentBase>(R"(
      type: object
      description: >
          a user-defined wrapper around api::GreeterServiceClient that provides
          a simplified interface.
      additionalProperties: false
      properties:
          endpoint:
              type: string
              description: >
                  Some other service endpoint (URI). 
      )");
      }

      TagRecommenderClient::TagRecommenderClient(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context)
        :  userver::components::LoggableComponentBase(config, component_context),
        client_factory_(
            component_context
                .FindComponent<userver::ugrpc::client::ClientFactoryComponent>()
                .GetFactory()),
        client_(client_factory_.MakeClient<tag_recommender::TagRecommenderClient>(
            "tag-recommender-client", config["endpoint"].As<std::string>())) {}
    

    std::vector<std::string> TagRecommenderClient::RecommendTags(int64_t note_id, std::vector<std::string>& existingTags) {
        tag_recommender::TagRecommendationRequest request;
        request.set_note_id(note_id);
        for (auto el : existingTags) {
            request.add_existing_tags(el);
        }

        auto response = client_.RecommendTags(request);
        std::vector<std::string> ans;
        for (auto el : *response.mutable_recommended_tags()) {
            ans.push_back(el);
        }
        return ans;
    }


    
   
}; // namespace nl::grpc::clients