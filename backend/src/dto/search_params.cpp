#include "search_params.hpp"

namespace nl::dto {

    SearchParams ParseSearchParams(const userver::server::http::HttpRequest& request,
                                    userver::server::request::RequestContext& context) {
    SearchParams searchParams;
    searchParams.user_id = context.GetData<int32_t>("user_id");

    if (request.HasArg("query")) {
        searchParams.query = request.GetArg("query");
        if (searchParams.query.size() > 500 || searchParams.query.size() < 1) {
            throw std::invalid_argument{"Invalid query size: less 1 or max(500) exeded"};
        }
    }
    if (request.HasArg("tags")) {
        searchParams.tags = request.GetArgVector("tags");
        if (searchParams.tags.size() > 32 || searchParams.tags.size() < 1) {
            throw std::invalid_argument{"Invalid tags size: less 1 or max(32) exeded"};
        }
    }
    if (request.HasArg("searchType")) {
        std::string searchTypeArg = request.GetArg("searchType");
        if (searchTypeArg == "semantic") {
            searchParams.searchType = SearchType::SEMANTIC;
        }
        else if (searchTypeArg == "exact") {
            searchParams.searchType = SearchType::EXACT;
        } else {
            throw std::invalid_argument{"Invalid search type"};
        }
    }
    if (request.HasArg("page")) {
        auto pageArg = request.GetArg("page");
        int page = std::stoi(pageArg);
        if (page < 1) {
            throw std::invalid_argument{"Invalid page: less 1"};
        }
        searchParams.page = page;

    }
    if (request.HasArg("limit")) {
        auto limitArg = request.GetArg("limit");
        int limit = std::stoi(limitArg);
        if (limit < 5 || limit > 20) {
            throw std::invalid_argument{"Invalid limit: less 5 or bigger 20"};
        }
        searchParams.limit = limit;

    }
    return searchParams;
}
} // namespace nl::dto