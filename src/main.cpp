#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "handlers/v1/register/view.hpp"
#include "handlers/v1/login/view.hpp"
#include "handlers/v1/get-events/view.hpp"
#include "handlers/v1/get-event-by-id/view.hpp"
#include "handlers/v1/response-to-join/view.hpp"
#include "handlers/v1/join_event/view.hpp"

int main(int argc, char* argv[]) {
  auto component_list = userver::components::MinimalServerComponentList()
                            .Append<userver::server::handlers::Ping>()
                            .Append<userver::components::TestsuiteSupport>()
                            .Append<userver::components::HttpClient>()
                            .Append<userver::server::handlers::TestsControl>()
                            .Append<userver::clients::dns::Component>()
                            .Append<userver::components::Postgres>("postgres-db-1");

  cooplan::AppendRegisterUser(component_list);
  cooplan::AppendLoginUser(component_list);
  cooplan::AppendGetEvents(component_list);
  cooplan::AppendGetEvent(component_list);
  cooplan::AppendResponseToJoin(component_list);
  cooplan::AppendJoinEvent(component_list);

  return userver::utils::DaemonMain(argc, argv, component_list);
}
