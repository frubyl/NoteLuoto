#pragma once
#include <string>
#include <samples/SyncService_client.usrv.pb.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/loggable_component_base.hpp>
#include <userver/components/component.hpp>
#include <userver/ugrpc/client/client_factory_component.hpp>

namespace nl::grpc::clients  {
    class NoteSyncClient final  : public userver::components::LoggableComponentBase {
        public:
            static constexpr std::string_view kName = "note-sync-client";

            NoteSyncClient(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);
            void AddNote(int64_t note_id, std::string note_text) ;
            void UpdateNote(int64_t note_id, std::string note_text) ;
            void DeleteNote(int64_t note_id) ;

            static userver::yaml_config::Schema GetStaticConfigSchema();
            
        private:
            userver::ugrpc::client::ClientFactory& client_factory_;
            notes::sync::SyncServiceClient client_;

        };
}; // namespace nl::grpc::clients