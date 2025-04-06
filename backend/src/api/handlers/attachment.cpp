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
        if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
            return{};
        }
    dto::AttachmentRequest attachment_request;
    try {
        attachment_request = dto::ParseAttachmentRequest(request);
    } catch(std::exception& ex) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
        return buildErrorMessage(ex.what());
    }

    if(attachment_request.file_.value.size() > 524288000 || attachment_request.file_.value.size() == 0) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
        return buildErrorMessage("Error file size: exeded or empty");
    }
    

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
        return buildErrorMessage("Filesystem error");
    }
    auto fileName = attachment_request.file_.filename ? attachment_request.file_.filename.value() : "";
    const auto result_add =
        cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster, db::sql::kAddAttachmentToNode.data(), newFileName, fileName, note_id);
    
    userver::formats::json::ValueBuilder response_body;
    response_body["attchment_id"] = result_add.AsSingleRow<int32_t>();

    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
    return ToString(response_body.ExtractValue());
}
std::string Handler::buildErrorMessage(std::string message) const {
    userver::formats::json::ValueBuilder response_body;
    response_body["message"] = message;
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
        if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
            return{};
        }
    dto::AttachmentRequest attachment_request;
    try {
        attachment_request = dto::ParseAttachmentRequest(request);
    } catch(std::exception& ex) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
        return buildErrorMessage(ex.what());
    }
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
        return buildErrorMessage("Filesystem error");
    }

    userver::formats::json::ValueBuilder response_body;
    response_body["file_name"] = attachment.old_file_name_;
    response_body["content"] = content;
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
    return response_body.ExtractValue();
}
userver::formats::json::Value Handler::buildErrorMessage(std::string message) const {
    userver::formats::json::ValueBuilder response_body;
    response_body["message"] = message;
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
        if (request.GetMethod() == userver::server::http::HttpMethod::kOptions) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
            return{};
        }
    dto::AttachmentRequest attachment_request;
    try {
        attachment_request = dto::ParseAttachmentRequest(request);
    } catch(std::exception& ex) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);  
        return buildErrorMessage(ex.what());
    }
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
        return buildErrorMessage("Filesystem error");
    }

    const auto result_delete =
        cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster, db::sql::kDeleteAttachment.data(), attachment_request.attachment_id_);
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
    return {};
}
userver::formats::json::Value Handler::buildErrorMessage(std::string message) const {
    userver::formats::json::ValueBuilder response_body;
    response_body["message"] = message;
    return response_body.ExtractValue();
  } 
} // namespace del

} // namespace nl::handlers::api::attachment
