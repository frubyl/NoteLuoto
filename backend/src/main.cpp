#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>
#include <userver/clients/dns/component.hpp>



#include "api/auth/auth_bearer.hpp"
#include "api/handlers/register.hpp"
#include "api/handlers/login.hpp"
#include "api/handlers/history.hpp"
#include "api/handlers/create_note.hpp"
#include "api/handlers/notes_id.hpp"
#include "api/handlers/tags_create.hpp"
#include "api/handlers/tags_all.hpp"
#include "api/handlers/tags_note.hpp"
#include "api/handlers/tags_note_tag.hpp"
#include "api/handlers/checklist.hpp"
#include "api/handlers/checklist_item.hpp"


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
          .Append<userver::components::HttpClient>()
          .Append<userver::components::DefaultSecdistProvider>()

          // Добавляем эндпоинты
          .Append<handlers::api::checklist::item::post::Handler>()
          .Append<handlers::api::checklist::item::put::Handler>()
          .Append<handlers::api::checklist::item::del::Handler>()

          .Append<handlers::api::checklist::note::post::Handler>()
          .Append<handlers::api::checklist::put::Handler>()
          .Append<handlers::api::checklist::del::Handler>()
          .Append<handlers::api::checklist::get::Handler>()
          
          .Append<handlers::api::reg::post::Handler>()
          .Append<handlers::api::login::post::Handler>()
          .Append<handlers::api::history::get::Handler>()
          .Append<handlers::api::tags::note::get::Handler>()
          .Append<handlers::api::tags::create::post::Handler>()
          .Append<handlers::api::tags::all::get::Handler>()
          .Append<handlers::api::tags::note::tag::post::Handler>()
          .Append<handlers::api::tags::note::tag::del::Handler>()
          .Append<handlers::api::notes::id::get::Handler>()
          .Append<handlers::api::notes::id::patch::Handler>()
          .Append<handlers::api::notes::post::Handler>();
  return userver::utils::DaemonMain(argc, argv, component_list);
}
