#include "../../db/sql.hpp"
#include "../../dto/checklist.hpp"
#include "../../models/checklist.hpp"
#include "checklist.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

namespace nl::handlers::api::checklist {


namespace note::post {
Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context),
      cluster_(context
                   .FindComponent<userver::components::Postgres>(
                       "postgres-db-1")
                   .GetCluster()),   
    client_(context.FindComponent<grpc::clients::NoteSyncClient>()),
    dataToTextFormatter_(config, context){}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context) const  {
        auto checklist = dto::ParseChecklistRequest(request);
        const auto result_find =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetNote.data(), checklist.note_id_);

        if (result_find.IsEmpty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
            return {};
        }

        const auto result_create =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kCreateChecklist.data(), checklist.note_id_, checklist.title_);

        std::string noteToText = dataToTextFormatter_.FormatNote(checklist.note_id_);
        try {
            client_.UpdateNote(checklist.note_id_, noteToText); 
        } catch(...) {}
        
           // Формируем тело ответа 
        userver::formats::json::ValueBuilder response_body;
        const auto result_set = result_create.AsSetOf<int32_t>();
        int32_t checklist_id = *(result_set.begin());
        response_body["checklist_id"] = checklist_id;  
        request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);  
       return response_body.ExtractValue();
}    
} // namespace note::post 

namespace get {
Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context),
      cluster_(context
                   .FindComponent<userver::components::Postgres>(
                       "postgres-db-1")
                   .GetCluster()){}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context) const  {
        auto checklist = dto::ParseChecklistRequest(request);
        const auto result_find =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetChecklist.data(), checklist.checklist_id_);

        if (result_find.IsEmpty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
            return {};
        }
        const auto result_checklist =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetChecklist.data(), checklist.checklist_id_);
        const auto result_items =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetAllChecklistItems.data(), checklist.checklist_id_);

        // Формируем тело ответа 
        userver::formats::json::ValueBuilder response_body = result_checklist.AsSingleRow<models::Checklist>(
            userver::storages::postgres::kRowTag);
        response_body["items"] = userver::formats::common::Type::kArray;  

        for (const auto& row : result_items) {
            response_body["items"].PushBack(row.As<models::Item>(
            userver::storages::postgres::kRowTag));
        }
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
        return response_body.ExtractValue();
}    
} // namespace get

namespace del {
Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context),
      cluster_(context
                   .FindComponent<userver::components::Postgres>(
                       "postgres-db-1")
                   .GetCluster()), 
                   client_(context.FindComponent<grpc::clients::NoteSyncClient>()),
                   dataToTextFormatter_(config, context){}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context) const  {
        auto checklist = dto::ParseChecklistRequest(request);
        const auto result_find =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetChecklist.data(), checklist.checklist_id_);

        if (result_find.IsEmpty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
            return {};
        }

        // Обновление 
        auto note_id = getNoteId(checklist.checklist_id_);
        std::string noteToText = dataToTextFormatter_.FormatNote(note_id);
        try {
            client_.UpdateNote(note_id, noteToText); 
        } catch(...) {}

        const auto result = 
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                    db::sql::kDeleteChecklist.data(), checklist.checklist_id_);

        
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
        return {};
    }

    int32_t Handler::getNoteId(int32_t checklist_id) const {
        const auto result = 
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetNoteIdByChecklistId.data(), checklist_id);
        return result.AsSingleRow<int32_t>();
        
    }
} // namespace del

namespace patch {
Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context),
      cluster_(context
                   .FindComponent<userver::components::Postgres>(
                       "postgres-db-1")
                   .GetCluster()),   client_(context.FindComponent<grpc::clients::NoteSyncClient>()),
                   dataToTextFormatter_(config, context){}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context) const  {
        auto checklist = dto::ParseChecklistRequest(request);
        const auto result_find =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetChecklist.data(), checklist.checklist_id_);

        if (result_find.IsEmpty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
            return {};
        }
        const auto result =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kUpdateChecklist.data(), checklist.title_, checklist.checklist_id_);
        
        // Обновление 
        auto note_id = getNoteId(checklist.checklist_id_);
        std::string noteToText = dataToTextFormatter_.FormatNote(note_id);
        try {
            client_.UpdateNote(note_id, noteToText); 
        } catch(...) {}


        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
        return {};
}
    int32_t Handler::getNoteId(int32_t checklist_id) const {
        const auto result = 
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetNoteIdByChecklistId.data(), checklist_id);
        return result.AsSingleRow<int32_t>();
    
}
} // namespace patch
} // namespace nl::handlers::api::checklist

 