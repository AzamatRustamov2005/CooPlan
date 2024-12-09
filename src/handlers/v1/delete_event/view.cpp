#include "view.hpp"
#include "../../lib/auth.hpp"

#include <fmt/format.h>

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/components/component_context.hpp>
#include <userver/utils/assert.hpp>
#include <userver/crypto/hash.hpp>
#include <stdexcept>

namespace cooplan {

namespace {
    static std::string ErrorResponse(const std::string& message) {
        userver::formats::json::ValueBuilder error_response;
        error_response["message"] = message;
        return userver::formats::json::ToString(error_response.ExtractValue());
    }
    class DeleteEvent final : public userver::server::handlers::HttpHandlerBase {
    public:
        static constexpr std::string_view kName = "handler-delete-event";

        DeleteEvent(const userver::components::ComponentConfig &config,
                    const userver::components::ComponentContext &component_context)
                : HttpHandlerBase(config, component_context),
                  pg_cluster_(
                      component_context
                          .FindComponent<userver::components::Postgres>("postgres-db-1")
                          .GetCluster()) {}

        std::string HandleRequestThrow(
            const userver::server::http::HttpRequest &request,
            userver::server::request::RequestContext &) const override {

            const auto id = request.GetPathArg("id");

            int event_id;

            try {
              event_id = std::stoi(id);
            }
            catch (const std::invalid_argument &e) {
              request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
              return ErrorResponse("Event ID is required");
            }

            auto session_info = cooplan::GetSessionInfo(pg_cluster_, request);
            if (!session_info.has_value()) {
                request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
                return ErrorResponse("Invalid or missing access token");
            }

            const auto user_id = session_info->user_id;

            auto result = pg_cluster_->Execute(
                userver::storages::postgres::ClusterHostType::kMaster,
                "SELECT organizer_id FROM cooplan.events WHERE id = $1",
                event_id);

            if (result.IsEmpty()) {
                request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
                return ErrorResponse("Event not found");
            }

            const auto organizer_id = result.AsSingleRow<int>();
            if (organizer_id != user_id) {
                request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
                return ErrorResponse("Unauthorized: User is not the organizer of this event");
            }

            pg_cluster_->Execute(
                userver::storages::postgres::ClusterHostType::kMaster,
                "DELETE FROM cooplan.events WHERE id = $1",
                event_id);

            userver::formats::json::ValueBuilder response;
            response["status"] = "deleted";
            request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
            return userver::formats::json::ToString(response.ExtractValue());
        }

    private:
        userver::storages::postgres::ClusterPtr pg_cluster_;
    };

}  // namespace

void AppendDeleteEvent(userver::components::ComponentList &component_list) {
    component_list.Append<DeleteEvent>();
}

}  // namespace cooplan
