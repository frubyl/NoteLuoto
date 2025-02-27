#include "../../db/sql.hpp"
#include "../../dto/note.hpp"
#include "../../models/note.hpp"
#include "../../models/attachment.hpp"
#include "note.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/logging/log.hpp>
#include <userver/utils/async.hpp>
#include <userver/engine/sleep.hpp>
#include <filesystem>

namespace nl::handlers::api::note {

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
        int32_t note_id = std::stoi(request.GetPathArg("note_id"));
        const auto result =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetNote.data(), note_id);

        if (result.IsEmpty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
            return {};
        }

        // Формируем тело ответа 
        auto note = result.AsSingleRow<models::NoteWithoutId>(userver::storages::postgres::kRowTag);

        userver::formats::json::ValueBuilder response_body;
        response_body = note;
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
        return response_body.ExtractValue();
}    
} // namespace get

namespace patch {
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

        auto noteRequest = dto::ParseNoteRequest(request, context);

        const auto result_get =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetNote.data(), noteRequest.note_id_);

        if (result_get.IsEmpty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
            return {};
        }
        
        if (noteRequest.title_ != "") {
            const auto result_update =  
                cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                    db::sql::kUpdateNoteTitle.data(), noteRequest.title_, noteRequest.note_id_);
        }
        if (noteRequest.body_ != "") {
            const auto result_update =  
                cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                    db::sql::kUpdateNoteBody.data(), noteRequest.body_, noteRequest.note_id_);
        }
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
        return {};
}       
} // namespace patch


namespace post {

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

    auto noteRequest = dto::ParseNoteRequest(request, context);

    // Создаем заметку
    const auto result =
    cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                    db::sql::kCreateNote.data(), noteRequest.title_, noteRequest.body_, noteRequest.user_id_);

    // Формируем тело ответа 
    userver::formats::json::ValueBuilder response_body;
    const auto result_set = result.AsSetOf<int32_t>();
    int32_t note_id = *(result_set.begin());
    response_body["note_id"] = note_id;  
    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);  
    return response_body.ExtractValue();
}   
} // namespace post

namespace del {
    bool tryDeleteFile(const std::filesystem::path& filePath, int maxRetries = 3, int delayMs = 1000) {
        int attempt = 0;
        while (attempt < maxRetries) {
            try {
                if (std::filesystem::remove(filePath)) return true;  
            }
            catch (...) {
            }
            userver::engine::SleepFor(std::chrono::milliseconds(delayMs));
            ++attempt;
        }
        return false;  
    }
    
    // Функция для удаления вложений
    void deleteAttachments(const std::vector<std::string>& attachments) {
        for (const auto& fileName : attachments) {
            std::filesystem::path filePath("attachments/" + fileName);
    
            if (std::filesystem::exists(filePath)) {
                if (tryDeleteFile(filePath)) {
                    LOG_INFO() << "File deleted: " <<  fileName;
                } else {
                    LOG_ERROR() << "Cant delete file: " <<  fileName;
                }
            } else {
                LOG_INFO() << "File does not exist: " <<  fileName;
            }
        }
    }
    
    Handler::Handler(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context),
    cluster_(context
            .FindComponent<userver::components::Postgres>(
                "postgres-db-1")
            .GetCluster()){
            }

    userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context) const  {

    int32_t note_id = std::stoi(request.GetPathArg("note_id"));

    // Создаем заметку
    const auto result_get =
            cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                                db::sql::kGetNote.data(), note_id);
    if (result_get.IsEmpty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
        return {};
    }
    const auto result_get_attachments =
        cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster,
                            db::sql::kGetAllNoteAttachments.data(), note_id);

    std::vector<std::string> attachments;
    for (auto &row : result_get_attachments) {
        attachments.push_back(row["file_name"].As<std::string>());
    }

    userver::utils::Async("Delete user attachments", deleteAttachments, attachments).Detach();
    cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster, db::sql::kDeleteNote.data(), note_id);
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
    return {};
    }   
} // namespace del

} // namespace nl::handlers::api::note


 