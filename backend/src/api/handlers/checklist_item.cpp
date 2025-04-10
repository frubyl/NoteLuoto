#include "../../db/sql.hpp"
#include "../../dto/item.hpp"
#include "checklist_item.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/async.hpp>

namespace nl::handlers::api::checklist::item {

namespace post {
Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context),
      cluster_(context
                   .FindComponent<userver::components::Postgres>(
                       "postgres-db-1")
                   .GetCluster()),
                   client_(context.FindComponent<grpc::clients::NoteSyncClient>()),
                   dataToTextFormatter_(cluster_){}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context) const  {
        if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
            return{};
        }
        dto::ItemRequest item;
        try {
            item = dto::ParseItemRequest(request);
        } catch(std::exception& ex) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
            return buildErrorMessage(ex.what());
        }        
        const auto result_find =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetChecklist.data(), item.checklist_id_);

        if (result_find.IsEmpty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
            return {};
        }
        // Обновление 
        auto note_id = getNoteId(item.checklist_id_);
        std::string noteToText = dataToTextFormatter_.FormatNote(note_id);
        userver::utils::Async("Send update note request to Ai-service",[noteToText = noteToText, note_id = note_id, &client = client_]() {
            try {
                client.UpdateNote(note_id, noteToText); 
            } catch (std::exception& ex) {
                LOG_ERROR() << ex.what();
            }
    
        }).Detach();   
        const auto result_create =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kCreateChecklistItem.data(), item.checklist_id_, item.text_);

        userver::formats::json::ValueBuilder response_body;
        int32_t item_id = result_create.AsSingleRow<int32_t>();
        response_body["item_id"] = item_id;  
        request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);  
        return response_body.ExtractValue();
}    

    int32_t Handler::getNoteId(int32_t checklist_id) const {
        const auto result = 
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetNoteIdByChecklistId.data(), checklist_id);
        return result.AsSingleRow<int32_t>();
            }
    userver::formats::json::Value Handler::buildErrorMessage(std::string message) const {
        userver::formats::json::ValueBuilder response_body;
        response_body["message"] = message;
        return response_body.ExtractValue();
        }  
} // namespace post

namespace patch {
Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context),
      cluster_(context
                   .FindComponent<userver::components::Postgres>(
                       "postgres-db-1")
                   .GetCluster()) ,
                   client_(context.FindComponent<grpc::clients::NoteSyncClient>()),
                   dataToTextFormatter_(cluster_){}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context) const  {
        if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
            return{};
        }
        dto::ItemRequest item;
        try {
            item = dto::ParseItemRequest(request);
        } catch(std::exception& ex) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
            return buildErrorMessage(ex.what());
        }    
        const auto result_find =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetChecklistItem.data(), item.item_id_);

        if (result_find.IsEmpty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
            return {};
        }

        if (item.text_ != "") {
            const auto result = 
                cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                    db::sql::kUpdateCheckListItemText.data(), item.text_, item.item_id_);
        }

        if (item.status_) {
            const auto result = 
                cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                    db::sql::kUpdateCheckListItemStatus.data(), item.status_.value(), item.item_id_);
        }

        // Обновление 
        auto note_id = getNoteId(item.item_id_);
        std::string noteToText = dataToTextFormatter_.FormatNote(note_id);
        userver::utils::Async("Send update note request to Ai-service",[noteToText = noteToText, note_id = note_id, &client = client_]() {
            try {
                client.UpdateNote(note_id, noteToText); 
            } catch (std::exception& ex) {
                LOG_ERROR() << ex.what();
            }
    
        }).Detach();  
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
        return {};
}    
    int32_t Handler::getNoteId(int32_t item_id) const {
        const auto result = 
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetNoteIdByItemId.data(), item_id);
        return result.AsSingleRow<int32_t>();
    }
    userver::formats::json::Value Handler::buildErrorMessage(std::string message) const {
        userver::formats::json::ValueBuilder response_body;
        response_body["message"] = message;
        return response_body.ExtractValue();
        }  
} // namespace patch

namespace del {
Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context),
      cluster_(context
                   .FindComponent<userver::components::Postgres>(
                       "postgres-db-1")
                   .GetCluster()),
                   client_(context.FindComponent<grpc::clients::NoteSyncClient>()),
                   dataToTextFormatter_(cluster_){}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context) const  {
        if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
            return{};
        }
        dto::ItemRequest item;
        try {
            item = dto::ParseItemRequest(request);
        } catch(std::exception& ex) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
            return buildErrorMessage(ex.what());
        }    
        const auto result_find =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetChecklistItem.data(), item.item_id_);

        if (result_find.IsEmpty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
            return {};
        }
          // Обновление 
          auto note_id = getNoteId(item.item_id_);
          std::string noteToText = dataToTextFormatter_.FormatNote(note_id);
          userver::utils::Async("Send update note request to Ai-service",[noteToText = noteToText, note_id = note_id, &client = client_]() {
            try {
                client.UpdateNote(note_id, noteToText); 
            } catch (std::exception& ex) {
                LOG_ERROR() << ex.what();
            }
    
        }).Detach();    
        const auto result =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kDeleteChecklistItem.data(), item.item_id_);

      
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
        return {};
}    
    int32_t Handler::getNoteId(int32_t item_id) const {
        const auto result = 
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetNoteIdByItemId.data(), item_id);
        return result.AsSingleRow<int32_t>();
            }
    userver::formats::json::Value Handler::buildErrorMessage(std::string message) const {
        userver::formats::json::ValueBuilder response_body;
        response_body["message"] = message;
        return response_body.ExtractValue();
        }  
} // namespace del

} // namespace nl::handlers::api::checklist::item

 