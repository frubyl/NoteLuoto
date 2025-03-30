#include "grpc_langchain_client.hpp"
#include <userver/clients/dns/component.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

namespace nl::grpc::clients {
    userver::yaml_config::Schema LangchainClient::GetStaticConfigSchema() {
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

    LangchainClient::LangchainClient(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context)
        :  userver::components::LoggableComponentBase(config, component_context),
        client_factory_(
            component_context
                .FindComponent<userver::ugrpc::client::ClientFactoryComponent>()
                .GetFactory()),
        client_(client_factory_.MakeClient<langchain::LangChainClient>(
            "langchain-client", config["endpoint"].As<std::string>())) {}
    

    std::string LangchainClient::GenerateAnswer(const std::string& query) {
       langchain::UserQuery request;
       request.set_query_text(std::move(query));
       auto response = client_.GenerateAnswer(request);
       return std::move(*response.mutable_answer_text());
    }

    
    std::vector<int64_t> LangchainClient::SemanticSearch(const std::string& query) {
       langchain::UserQuery request;
       request.set_query_text(std::move(query));
       auto response = client_.SemanticSearch(request);

       std::vector<int64_t> ans;
       for (auto el : *response.mutable_notes_id()) {
            ans.push_back(el);
       }
       return ans;
    }
    
    std::vector<std::string> LangchainClient::RecommendPrompts() {
        langchain::EmptyRequest request;
        auto response = client_.RecommendPrompts(request);
        std::vector<std::string> ans;
        for (auto el : *response.mutable_recommended_prompts()) {
             ans.push_back(el);
        }
 
        return ans;    
    };
}; // namespace nl::grpc::clients