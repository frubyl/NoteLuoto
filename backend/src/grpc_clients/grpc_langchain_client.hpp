#pragma once
#include <string>
#include <cstdint>
#include <samples/Langchain_client.usrv.pb.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/components/component.hpp>
#include <userver/ugrpc/client/client_factory_component.hpp>


namespace nl::grpc::clients  {
    class LangchainClient final  : public userver::components::LoggableComponentBase {
        public:
            static constexpr std::string_view kName = "langchain-client";

            LangchainClient(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);
           
            std::string GenerateAnswer(const std::string& query) ;
            std::vector<int64_t> SemanticSearch(const std::string& query) ;
            std::vector<std::string> RecommendPrompts() ;

            static userver::yaml_config::Schema GetStaticConfigSchema();
            
        private:
            userver::ugrpc::client::ClientFactory& client_factory_;
            langchain::LangChainClient client_;

        };
}; // namespace nl::grpc::clients