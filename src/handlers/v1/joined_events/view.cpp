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
    static std::string BadResponse(const userver::server::http::HttpRequest &request, const std::string &message) {
        userver::formats::json::ValueBuilder error_response;
        error_response["message"] = message;
        return userver::formats::json::ToString(error_response.ExtractValue());
    }

    class JoinedEvents final : public userver::server::handlers::HttpHandlerBase {
    public:
        static constexpr std::string_view kName = "handler-joined-events";

        JoinedEvents(const userver::components::ComponentConfig &config,
                     const userver::components::ComponentContext &component_context)
                : HttpHandlerBase(config, component_context),
                  pg_cluster_(
                      component_context
                          .FindComponent<userver::components::Postgres>("postgres-db-1")
                          .GetCluster()) {}

        std::string HandleRequestThrow(
            const userver::server::http::HttpRequest &request,
            userver::server::request::RequestContext &) const override {

            auto session_info = cooplan::GetSessionInfo(pg_cluster_, request);
            if (!session_info.has_value()) {
                request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
                return BadResponse(request, "Invalid or missing access token");
            }
            const auto user_id = session_info->user_id;

            auto result = pg_cluster_->Execute(
                userver::storages::postgres::ClusterHostType::kMaster,
                R"(
                    SELECT ep.event_id, e.title AS event_name
                    FROM cooplan.event_participant AS ep
                    INNER JOIN cooplan.events AS e ON ep.event_id = e.id
                    WHERE ep.user_id = $1 AND ep.status = 'accepted'::public.status_enum
                )",
                user_id);

            // Log the number of rows returned
            LOG_INFO() << "Number of rows fetched: " << result.Size();
            LOG_INFO() << "Organizer ID from session: " << user_id;

            userver::formats::json::ValueBuilder response = userver::formats::json::Type::kArray;

            for (const auto& row : result) {
                userver::formats::json::ValueBuilder item;
                item["event_id"] = row["event_id"].As<int>();
                item["event_name"] = row["event_name"].As<std::string>();
                response.PushBack(std::move(item));
            }

            return userver::formats::json::ToString(response.ExtractValue());
        }

    private:
        userver::storages::postgres::ClusterPtr pg_cluster_;
    };

}  // namespace

void AppendJoinedEvents(userver::components::ComponentList &component_list) {
    component_list.Append<JoinedEvents>();
}

}  // namespace cooplan
