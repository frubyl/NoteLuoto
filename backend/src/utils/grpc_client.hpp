// #pragma once
// #include <string>
// #include <handlers/hello_client.usrv.pb.hpp>
// #include <userver/components/component_list.hpp>
// #include <userver/components/loggable_component_base.hpp>
// #include <userver/components/component.hpp>
// #include <userver/ugrpc/client/client_factory_component.hpp>

// namespace nl::utils::grpc {
//     class GreeterClient final : public components::LoggableComponentBase {
//     public:
//     static constexpr std::string_view kName = "greeter-client";

//     GreeterClient(const components::ComponentConfig& config,
//                     const components::ComponentContext& context);

//     std::string GetAnswer(std::string query);
//     std::string GetAnswer(std::string query);

//     static yaml_config::Schema GetStaticConfigSchema();

//     private:
//     ugrpc::client::ClientFactory& client_factory_;
//     api::GreeterServiceClient client_;
// };
// } // namespace nl::utils::grpc 