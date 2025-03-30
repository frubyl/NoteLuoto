#include "../../db/sql.hpp"
#include "../../dto/attachment.hpp"
#include "../../models/attachment.hpp"

#include "attachment.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <filesystem>
#include <userver/utils/async.hpp>
#include <fstream>
#include <userver/fs/write.hpp>
#include <exception> 
#include <userver/logging/log.hpp>
#include "../../utils/file_manager.hpp"

namespace nl::handlers::api::attachment {

namespace post {
Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      cluster_(context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster()) {}

std::string Handler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {

    auto attachment_request = dto::ParseAttachmentRequest(request);
    auto note_id = attachment_request.note_id_;
    const auto result_search =
        cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster, db::sql::kGetNote.data(), note_id);

    if (result_search.IsEmpty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
        return {}; 
    }

    std::string newFileName;

    try {
        newFileName = utils::FileManager::SaveFile(attachment_request.file_);
    } catch (...) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kInternalServerError);  
        return {};  
    }
    auto fileName = attachment_request.file_.filename ? attachment_request.file_.filename.value() : "";
    const auto result_add =
        cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster, db::sql::kAddAttachmentToNode.data(), newFileName, fileName, note_id);
    
    const auto result_set = result_add.AsSetOf<int32_t>();
    int32_t attachment_id = *(result_set.begin());
    userver::formats::json::ValueBuilder response_body;
    response_body["attchment_id"] = attachment_id;

    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
    return ToString(response_body.ExtractValue());
}

} // namespace post

namespace get {

Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context),
      cluster_(context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster()) {}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context) const {

    auto attachment_request = dto::ParseAttachmentRequest(request);
    const auto result =
        cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster, db::sql::kGetAttachmentNames.data(), attachment_request.attachment_id_);
    if (result.IsEmpty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
        return {}; 
    }

    auto attachment = result.AsSingleRow<models::Attachment>(userver::storages::postgres::kRowTag);
    std::string content;
    try {
        content = utils::FileManager::ReadFile(attachment.file_name_);
    } catch (...) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kInternalServerError);  
        return {};  
    }

    userver::formats::json::ValueBuilder response_body;
    response_body["file_name"] = attachment.old_file_name_;
    response_body["content"] = content;
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
    return response_body.ExtractValue();
}

} // namespace get

namespace del {

Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context),
      cluster_(context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster()) {}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext& context) const {

    auto attachment_request = dto::ParseAttachmentRequest(request);
    const auto result =
        cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster, db::sql::kGetAttachmentNames.data(), attachment_request.attachment_id_);
    if (result.IsEmpty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);  
        return {};             
    }

    auto attachment = result.AsSingleRow<models::Attachment>(userver::storages::postgres::kRowTag);
    try {
        utils::FileManager::DeleteFile(attachment.file_name_);
    } catch(...) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kInternalServerError);  
        return {};  
    }

    const auto result_delete =
        cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster, db::sql::kDeleteAttachment.data(), attachment_request.attachment_id_);
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
    return {};
}

} // namespace del

} // namespace nl::handlers::api::attachment
