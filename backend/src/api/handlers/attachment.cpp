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

namespace nl::handlers::api::attachment {

namespace post {

std::string generateRandomFilename(const std::optional<std::string>& extension) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 200);

    std::stringstream filename;
    for (int i = 0; i < 50; ++i) {
        filename << std::hex << dis(gen);
    }
    if (extension) {
        filename << "." << extension.value();
    }
    return filename.str();
}

std::optional<std::string> getFileExtension(const std::optional<std::string>& filename) {
    if (!filename) {
        return std::nullopt;
    }
    size_t pos = filename.value().rfind('.');
    if (pos != std::string::npos && pos != 0) {
        return filename.value().substr(pos + 1);
    }
    return std::nullopt;
}

void saveFile(std::string_view value, std::string fileName) {
    std::fstream outFile("attachments/" + fileName, std::fstream::binary | std::fstream::out);

    if (!outFile.is_open()) {
        LOG_ERROR() << "Failed to open file for writing";
        throw std::runtime_error("Failed to open file for writing");
    }
    outFile << value;

    if (outFile.fail()) {
        LOG_ERROR() << "Error writing to file";
        throw std::runtime_error("Error writing to file");
    }

    outFile.close();
}

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

    auto file = attachment_request.file_;
    auto extension = getFileExtension(file.filename);
    auto fileNameNew = generateRandomFilename(extension);

    try {
        userver::utils::Async("Save user file", saveFile, file.value, fileNameNew).Wait();
    } catch (...) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kInternalServerError);  
        return {};  
    }

    const auto result_add =
        cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlaveOrMaster, db::sql::kAddAttachmentToNode.data(), fileNameNew, file.filename, note_id);
    
    const auto result_set = result_add.AsSetOf<int32_t>();
    int32_t attachment_id = *(result_set.begin());
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);  
    return "{attachment_id:" + std::to_string(attachment_id) + "}";
}

} // namespace post

namespace get {

std::string readFile(std::string fileName) {
    std::ifstream inFile("attachments/" + fileName, std::ios::binary);

    if (!inFile.is_open()) {
        LOG_ERROR() << "Failed to open file for reading";
        throw std::runtime_error("Failed to open file for reading");
    }

    std::string content;
    char ch;
    while (inFile.get(ch)) {
        content += ch;
    }

    if (inFile.bad()) {
        inFile.close();
        LOG_ERROR() << "Error reading from file";
        throw std::runtime_error("Error reading from file");
    }

    inFile.close();
    return content;
}

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
        content = userver::utils::Async("Read user file", readFile, attachment.file_name_).Get();
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

void deleteFile(const std::string& fileName) {
    std::string path = "attachments/" + fileName;
    if (!std::filesystem::remove(path)) {
        LOG_ERROR() << "Failed to delete file: " << path;
        throw std::runtime_error("Failed to delete file");
    }
}

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
        userver::utils::Async("Delete user file", deleteFile, attachment.file_name_).Wait();
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
