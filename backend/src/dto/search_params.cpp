#include "search_params.hpp"

namespace nl::dto {

    SearchParams ParseSearchParams(const userver::server::http::HttpRequest& request,
                                    userver::server::request::RequestContext& context) {
    SearchParams searchParams;
    searchParams.user_id = context.GetData<int32_t>("user_id");

    if (request.HasArg("query")) {
        searchParams.query = request.GetArg("query");
    }
    if (request.HasArg("tags")) {
        searchParams.tags = request.GetArgVector("tags");
    }
    if (request.HasArg("searchType")) {
        std::string searchTypeArg = request.GetArg("searchType");
        if (searchTypeArg == "semantic") {
            searchParams.searchType = SearchType::SEMANTIC;
        }
        if (searchTypeArg == "exact") {
            searchParams.searchType = SearchType::EXACT;
        }
    }
    if (request.HasArg("page")) {
        searchParams.page = std::stoi(request.GetArg("page"));
    }
    if (request.HasArg("limit")) {
        searchParams.limit = std::stoi(request.GetArg("limit"));
    }
    return searchParams;
}
} // namespace nl::dto