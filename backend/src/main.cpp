#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>
#include <userver/clients/dns/component.hpp>

// Компоненты для grpc клиентов
#include <userver/ugrpc/client/client_factory_component.hpp>
#include <userver/ugrpc/client/common_component.hpp>
#include <userver/ugrpc/client/middlewares/deadline_propagation/component.hpp>
#include <userver/ugrpc/client/middlewares/log/component.hpp>
#include <userver/ugrpc/client/middlewares/base.hpp>
// Пользовательские компоненты 
#include "api/auth/auth_bearer.hpp"
#include "api/handlers/register.hpp"
#include "api/handlers/login.hpp"
#include "api/handlers/history.hpp"
#include "api/handlers/note.hpp"
#include "api/handlers/notes.hpp"
#include "api/handlers/suggest.hpp"
#include "api/handlers/tag.hpp"
#include "api/handlers/ai_answer.hpp"
#include "api/handlers/checklist.hpp"
#include "api/handlers/checklist_item.hpp"
#include "api/handlers/attachment.hpp"
#include "grpc_clients/grpc_sync_client.hpp"
#include "grpc_clients/grpc_langchain_client.hpp"
#include "grpc_clients/grpc_tag_recommender_client.hpp"
#include "utils/pipeline_builder.cpp"
#include <userver/ugrpc/client/component_list.hpp>
int main(int argc, char* argv[]) {
  using namespace nl;
  userver::server::handlers::auth::RegisterAuthCheckerFactory(
    "bearer", std::make_unique<handlers::auth::CheckerFactory>());
      auto component_list =
      userver::components::MinimalServerComponentList()
          .Append<userver::server::handlers::Ping>()
          .Append<userver::components::TestsuiteSupport>()
          .Append<userver::components::Postgres>("postgres-db-1")
          .Append<userver::server::handlers::TestsControl>()
          .Append<userver::components::Secdist>()
          .Append<userver::clients::dns::Component>()
          .Append<userver::components::DefaultSecdistProvider>()
          .Append<userver::components::HttpClient>()


          // Добавление компонентов для grpc клиентов 
          // .Append<userver::ugrpc::client::CommonComponent>()
          .Append<userver::ugrpc::client::ClientFactoryComponent>()
          // .Append<userver::ugrpc::client::middlewares::deadline_propagation::Component>()
          // .Append<userver::ugrpc::client::middlewares::log::Component>()
          // .Append<userver::ugrpc::client::MiddlewarePipelineComponent>()

          .AppendComponentList(userver::ugrpc::client::MinimalComponentList())

          
          .Append<utils::CustomHandlerPipelineBuilder>("custom-handler-pipeline-builder")
            .Append<utils::CORSMiddlewareFactory>()

          // Добавление grpc - клиентов
          .Append<nl::grpc::clients::NoteSyncClient>()
          .Append<nl::grpc::clients::LangchainClient>()
          .Append<nl::grpc::clients::TagRecommenderClient>()


          // Добавляем эндпоинты для работы с вложениями
          .Append<handlers::api::attachment::post::Handler>()
          .Append<handlers::api::attachment::get::Handler>()
          .Append<handlers::api::attachment::del::Handler>()

          // Добавляем эндпоинты для работы с пунктами чеклистов
          .Append<handlers::api::checklist::item::post::Handler>()
          .Append<handlers::api::checklist::item::patch::Handler>()
          .Append<handlers::api::checklist::item::del::Handler>()

          // Добавляем эндпоинты для работы с чеклистами
          .Append<handlers::api::checklist::note::post::Handler>()
          .Append<handlers::api::checklist::patch::Handler>()
          .Append<handlers::api::checklist::del::Handler>()
          .Append<handlers::api::checklist::get::Handler>()
          
          // Добавляем эндпоинты для входа и регистрации
          .Append<handlers::api::reg::post::Handler>()
          .Append<handlers::api::login::post::Handler>()

          // Добавляем эндпоинты для получения истории
          .Append<handlers::api::history::get::Handler>()

          // Добавляем эндпоинты для работы с тегами
          .Append<handlers::api::tag::note::get::Handler>()
          .Append<handlers::api::tag::create::post::Handler>()
          .Append<handlers::api::tag::all::get::Handler>()
          .Append<handlers::api::tag::note::post::Handler>()
          .Append<handlers::api::tag::note::del::Handler>()

          // Добавляем эндпоинты для работы с рекомендациями
          .Append<handlers::api::suggest::queries::Handler>()
          .Append<handlers::api::suggest::tags::Handler>()

          // Добавляем эндпоинты для получения ответа бота
          .Append<handlers::api::ai::answer::Handler>()

          // Добавляем эндпоинты для работы с заметками
          .Append<handlers::api::note::del::Handler>()
          .Append<handlers::api::note::get::Handler>()
          .Append<handlers::api::note::patch::Handler>()
          .Append<handlers::api::get::notes::Handler>()
          .Append<handlers::api::note::post::Handler>();

  return userver::utils::DaemonMain(argc, argv, component_list);
}
