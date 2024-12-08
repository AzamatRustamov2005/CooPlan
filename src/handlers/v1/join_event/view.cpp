#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include <userver/crypto/hash.hpp>

#include "../../../models/user.hpp"
#include "../../../common/response_helpers.hpp"
#include "../../lib/auth.hpp"

namespace cooplan {

    namespace {

        class JoinEvent final : public userver::server::handlers::HttpHandlerBase {
        public:
            static constexpr std::string_view
            kName = "handler-join-event";

            JoinEvent(const userver::components::ComponentConfig &config,
                      const userver::components::ComponentContext &component_context)
                    : HttpHandlerBase(config, component_context),
                      pg_cluster_(
                              component_context
                                      .FindComponent<userver::components::Postgres>("postgres-db-1")
                                      .GetCluster()) {}

            std::string HandleRequestThrow(
                    const userver::server::http::HttpRequest &request,
                    userver::server::request::RequestContext &
            ) const override {
                const auto request_body = userver::formats::json::FromString(request.RequestBody());
                
                auto session = GetSessionInfo(pg_cluster_, request);
                if (!session) {
                    auto& response = request.GetHttpResponse();
                    response.SetStatus(userver::server::http::HttpStatus::kUnauthorized);
                    return {};
                }
                const auto event_id = std::stoi(request.GetPathArg("id"));
                const auto user_id = session->user_id;
/*
                if (!event_id.has_value() || event_id->empty()) {
                    return common::GetBadResponse(request, "Event id cannot be empty");
                }
*/
                const auto result = pg_cluster_->Execute(
                        userver::storages::postgres::ClusterHostType::kMaster,
                        "INSERT INTO cooplan.event_participant(event_id, user_id) VALUES($1, $2) "
                        "ON CONFLICT DO NOTHING "
                        "RETURNING 'success'::text",
                        event_id, user_id
                );
            
                // TODO: Check end_datetime 

                if (result.IsEmpty()) {
                    return common::GetBadResponse(request, "User already joined this event");
                    /*
                    auto &response = request.GetHttpResponse();
                    response.SetStatus(userver::server::http::HttpStatus::kNotFound);
                    return {};
                    */
                }

                userver::formats::json::ValueBuilder response;
                response["status"] = result.AsSingleRow<std::string>();
                return userver::formats::json::ToString(response.ExtractValue());
            }

        private:
            userver::storages::postgres::ClusterPtr pg_cluster_;
        };

    }  // namespace

    void AppendJoinEvent(userver::components::ComponentList &component_list) {
        component_list.Append<JoinEvent>();
    }

}  // namespace cooplan
