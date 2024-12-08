#include "view.hpp"
#include "../../lib/auth.hpp"

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/components/component_context.hpp>
#include <userver/utils/assert.hpp>
#include <userver/crypto/hash.hpp>

namespace cooplan {

namespace {
    static std::string ErrorResponse(const std::string& message) {
        userver::formats::json::ValueBuilder error_response;
        error_response["message"] = message;
        return userver::formats::json::ToString(error_response.ExtractValue());
    }
    class RequestsForOrganizator final : public userver::server::handlers::HttpHandlerBase {
    public:
        static constexpr std::string_view kName = "handler-requests-for-organizator";

        RequestsForOrganizator(const userver::components::ComponentConfig &config,
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
                return ErrorResponse("Invalid or missing access token");
            }
            const auto organizer_id =session_info->user_id;

            auto result = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            R"(
                SELECT ep.event_id, e.title AS event_name, ep.user_id, u.username AS user_username
                FROM cooplan.event_participant AS ep
                INNER JOIN cooplan.events AS e ON ep.event_id = e.id
                INNER JOIN cooplan.users AS u ON ep.user_id = u.id
                WHERE ep.status = 'pending'::public.status_enum AND e.organizer_id = $1;
            )",
            organizer_id);

            // Log the number of rows returned
            LOG_INFO() << "Number of rows fetched: " << result.Size();
            LOG_INFO() << "Organizer ID from session: " << organizer_id;

            userver::formats::json::ValueBuilder response = userver::formats::json::Type::kArray;

            for (const auto& row : result) {
                userver::formats::json::ValueBuilder item;
                item["event_id"] = row["event_id"].As<int>();
                item["event_name"] = row["event_name"].As<std::string>();
                item["user_id"] = row["user_id"].As<int>();
                item["user_username"] = row["user_username"].As<std::string>();
                response.PushBack(std::move(item));
            }

            return userver::formats::json::ToString(response.ExtractValue());
        }

    private:
        userver::storages::postgres::ClusterPtr pg_cluster_;
    };

}  // namespace

void AppendRequestsForOrganizator(userver::components::ComponentList &component_list) {
    component_list.Append<RequestsForOrganizator>();
}

}  // namespace cooplan
