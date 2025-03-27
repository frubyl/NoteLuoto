#include "grpc_sync_client.hpp"
#include <userver/clients/dns/component.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

namespace nl::grpc::clients {
    userver::yaml_config::Schema NoteSyncClient::GetStaticConfigSchema() {
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

    NoteSyncClient::NoteSyncClient(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context)
        :  userver::components::LoggableComponentBase(config, component_context),
        client_factory_(
            component_context
                .FindComponent<userver::ugrpc::client::ClientFactoryComponent>()
                .GetFactory()),
        client_(client_factory_.MakeClient<notes::sync::SyncServiceClient>(
            "note-sync-client", config["endpoint"].As<std::string>())) {}
    
    void NoteSyncClient::AddNote(int64_t note_id, std::string note_text)  {
        notes::sync::NoteRequest request;
        request.set_note_id(note_id);
        request.set_full_text(note_text);
        client_.AddNote(request);    }

    void NoteSyncClient::UpdateNote(int64_t note_id, std::string note_text)  {
        notes::sync::NoteRequest request;
        request.set_note_id(note_id);
        request.set_full_text(note_text);
        client_.UpdateNote(request);    
    }

    void NoteSyncClient::DeleteNote(int64_t note_id)  {
        notes::sync::DeleteRequest request;
        request.set_note_id(note_id);
        client_.DeleteNote(request);    
    }

}; // namespace nl::grpc::clients