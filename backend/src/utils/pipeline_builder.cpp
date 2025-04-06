#include "cors_middleware.cpp"
#include <userver/server/middlewares/http_middleware_base.hpp>
#include <userver/server/middlewares/configuration.hpp>

namespace nl::utils {
class CustomHandlerPipelineBuilder final : public userver::server::middlewares::HandlerPipelineBuilder {
  public:
      using userver::server::middlewares::HandlerPipelineBuilder::HandlerPipelineBuilder;
   
      userver::server::middlewares::MiddlewaresList BuildPipeline(userver::server::middlewares::MiddlewaresList server_middleware_pipeline
      ) const override {
          auto& pipeline = server_middleware_pipeline;
          pipeline.emplace_back(CORSMiddleware::kName);
          return pipeline;
      }
  };
}