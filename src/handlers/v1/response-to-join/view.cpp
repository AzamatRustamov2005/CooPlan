#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include "../../lib/auth.hpp"
#include "../../../models/event.hpp"

namespace cooplan {

namespace {

    class ResponseToJoin final : public userver::server::handlers::HttpHandlerBase {
        public:
            static constexpr std::string_view kName = "handler-response-to-join";

            ResponseToJoin(const userver::components::ComponentConfig& config,
                           const userver::components::ComponentContext& component_context)
                           : HttpHandlerBase(config, component_context),
                                pg_cluster_(
                                    component_context
                                        .FindComponent<userver::components::Postgres>("postgres-db-1")
                                        .GetCluster()) {}

            std::string HandleRequestThrow(
                const userver::server::http::HttpRequest& request,
                userver::server::request::RequestContext&
            ) const override {
                auto session = GetSessionInfo(pg_cluster_, request);
                if (!session) {
                    auto& response = request.GetHttpResponse();
                    response.SetStatus(userver::server::http::HttpStatus::kUnauthorized);
                    return {};
                }

                auto request_body = userver::formats::json::FromString(request.RequestBody());
                auto event_id = request_body["event_id"].As<int>();
                auto user_id = request_body["user_id"].As<int>();
                auto status = request_body["status"].As<std::string>();

                auto result = pg_cluster_->Execute(
                    userver::storages::postgres::ClusterHostType::kMaster,
                    "UPDATE cooplan.event_participant "
                    "SET status = $3::status_enum "
                    "WHERE id = $1 AND user_id = $2",
                    event_id, user_id, status
                );

                return {};
            }

            private:
                userver::storages::postgres::ClusterPtr pg_cluster_;

        };

    }  // namespace

    void AppendResponseToJoin(userver::components::ComponentList& component_list) {
      component_list.Append<ResponseToJoin>();
    }

}  // namespace cooplan
