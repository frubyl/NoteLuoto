#include "history.hpp"

namespace nl::dto {
    
HistoryRequest ParseHistoryRequest(const userver::server::http::HttpRequest& request,
                                userver::server::request::RequestContext& context) {
    HistoryRequest historyRequest;
    historyRequest.user_id_ = context.GetData<int32_t>("user_id");

    auto page = request.GetArg("page");
    historyRequest.page_ = std::stoi(page);
    
    auto limit = request.GetArg("limit");
    historyRequest.limit_ = std::stoi(limit);

    return historyRequest;

}

}  // namespace nl::dto    
