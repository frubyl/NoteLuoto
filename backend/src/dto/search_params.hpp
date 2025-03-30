#pragma once
#include <string> 
#include <vector> 
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>

namespace nl::dto {

    enum class SearchType {             // Тип поиска
        EXACT,
        SEMANTIC,
        NONE
    };

    struct SearchParams {
        std::string query = "";
        std::vector<std::string> tags = {};
        SearchType searchType = SearchType::NONE;
        int64_t page = 1;
        int64_t limit = 20;
        int32_t user_id;
    };

    SearchParams ParseSearchParams(const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context);

}  // namespace nl::dto