#include <userver/server/middlewares/http_middleware_base.hpp>

namespace nl::utils {

class CORSMiddleware final: public userver::server::middlewares::HttpMiddlewareBase {
    public:
      static constexpr std::string_view kName{"cors-middleware"};
  
      explicit CORSMiddleware(const userver::server::handlers::HttpHandlerBase&) {
      }
  
    private:
      void HandleRequest(userver::server::http::HttpRequest& request, userver::server::request::RequestContext& context) const override {
        request.GetHttpResponse().SetHeader(std::string_view("Access-Control-Allow-Origin"), "*");
        request.GetHttpResponse().SetHeader(std::string_view("Access-Control-Allow-Methods"), "GET,POST,PUT,PATCH,DELETE,OPTIONS");
        request.GetHttpResponse().SetHeader(std::string_view("Access-Control-Allow-Headers"), "authorization, origin, content-type, accept");
        request.GetHttpResponse().SetHeader(std::string_view("Allow"), "HEAD,GET,POST,PUT,PATCH,DELETE,OPTIONS");
        if (request.GetMethod() != userver::server::http::HttpMethod::kOptions) {
          Next(request, context);
        } else {
          request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kOk);
        }
      }
    };
  
    using CORSMiddlewareFactory = userver::server::middlewares::SimpleHttpMiddlewareFactory<CORSMiddleware>;
}