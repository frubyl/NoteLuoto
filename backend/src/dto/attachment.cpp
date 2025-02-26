#include "attachment.hpp"

namespace nl::dto {
    
    AttachmentRequest ParseAttachmentRequest(const userver::server::http::HttpRequest& request) {
    AttachmentRequest attachmentRequest;
    try {
        attachmentRequest.file_ = request.GetFormDataArg("file");
    } catch(...){}
    if (request.HasPathArg("note_id")) {
        attachmentRequest.note_id_ = std::stoi(request.GetPathArg("note_id"));
    }
    if (request.HasPathArg("attachment_id")) {
        attachmentRequest.attachment_id_ = std::stoi(request.GetPathArg("attachment_id"));

    }
    return attachmentRequest;
}

}  // namespace nl::dto  