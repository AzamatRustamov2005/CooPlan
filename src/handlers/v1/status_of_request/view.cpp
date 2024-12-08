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

namespace cooplan {

namespace {
    static std::string ErrorResponse(const std::string& message) {
        userver::formats::json::ValueBuilder error_response;
        error_response["message"] = message;
        return userver::formats::json::ToString(error_response.ExtractValue());
    }

    class StatusOfRequest final : public userver::server::handlers::HttpHandlerBase {
    public:
        static constexpr std::string_view kName = "handler-status-of-request";

        StatusOfRequest(const userver::components::ComponentConfig &config,
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
            const auto user_id = session_info->user_id;

            const auto request_body = userver::formats::json::FromString(request.RequestBody());
            const auto event_id = request_body["event_id"].As<std::optional<int>>();
            if (!event_id.has_value()) {
                request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
                return ErrorResponse("Event id is required and must be an integer");
            }

            auto result = pg_cluster_->Execute(
                userver::storages::postgres::ClusterHostType::kMaster,
                "SELECT status::text AS status FROM cooplan.event_participant WHERE event_id = $1 AND user_id = $2",
                *event_id, user_id);

            if (result.IsEmpty()) {
                userver::formats::json::ValueBuilder response;
                response["status"] = "join";
                return userver::formats::json::ToString(response.ExtractValue());
            }

            const auto status = result.AsSingleRow<std::string>();
            
            userver::formats::json::ValueBuilder response;
            response["status"] = status;
            return userver::formats::json::ToString(response.ExtractValue());
        }

    private:
        userver::storages::postgres::ClusterPtr pg_cluster_;
    };

}  // namespace

void AppendStatusOfRequest(userver::components::ComponentList &component_list) {
    component_list.Append<StatusOfRequest>();
}

}  // namespace cooplan
