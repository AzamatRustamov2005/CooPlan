#include "view.hpp"
#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <string>

#include "../../../models/event.hpp"

namespace cooplan {
  namespace {
  std::string GetErrorResponse(const userver::server::http::HttpRequest& request, const std::string& message) {
    userver::formats::json::ValueBuilder error_response;
    error_response["message"] = message;

    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
    return userver::formats::json::ToString(error_response.ExtractValue());
  }
  class JoinedEvents final : public userver::server::handlers::HttpHandlerBase {
  public:
    static constexpr std::string_view kName = "handler-joined-events";

    JoinedEvents(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& component_context)
        : HttpHandlerBase(config, component_context),
          pg_cluster_(
              component_context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster()) {}

    std::string HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                   userver::server::request::RequestContext&) const override {
      const auto user_id = request.GetPathArg("id");
      auto result = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            "SELECT id, title, description, organizer_id, is_active, members_limit, start_datetime, finish_datetime, registration_deadline, latitude, longitude, image_url "
            "FROM cooplan.users WHERE id = $1",
            user_id);

      if (result.IsEmpty()) {
        return GetErrorResponse(request, "User not found.");
      }


      const auto events = result.AsSetOf<Event>(userver::storages::postgres::kRowTag);
      userver::formats::json::ValueBuilder response;

      response["joined_events"].Resize(0);

      for(const auto event : events){
        if (std::to_string(event.organizer_id) == user_id){
          continue;
        }else{
          response["joined_events"].PushBack(event);
        }
      }

      return userver::formats::json::ToString(response.ExtractValue());
    }
private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
  };

  }
  void AppendJoinedEvents(userver::components::ComponentList& component_list) {
    component_list.Append<JoinedEvents>();
  }
}