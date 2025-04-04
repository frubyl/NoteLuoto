#include "history.hpp"
#include <exception>
namespace nl::dto {
    
HistoryRequest ParseHistoryRequest(const userver::server::http::HttpRequest& request,
                                userver::server::request::RequestContext& context) {
    HistoryRequest historyRequest;
    historyRequest.user_id_ = context.GetData<int32_t>("user_id");

    if (request.HasArg("page")) {
        auto pageArg = request.GetArg("page");
        int page = std::stoi(pageArg);
        if (page < 1) {
            throw std::invalid_argument{"Invalid page: less 1"};
        }
        historyRequest.page_ = page;
    }

    if (request.HasArg("limit")) {
        auto limitArg = request.GetArg("limit");
        int limit = std::stoi(limitArg);
        if (limit < 5 || limit > 20) {
            throw std::invalid_argument{"Invalid limit: less 5 or bigger 20"};
        }
        historyRequest.limit_ = limit;
    }
    return historyRequest;

}

}  // namespace nl::dto    
