#include "view.hpp"
#include "../../../models/event.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/crypto/hash.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

namespace cooplan {

    namespace {

        class GetEvent final : public userver::server::handlers::HttpHandlerBase {
            public:
                static constexpr std::string_view
                kName = "handler-get-event-by-id";

                GetEvent(const userver::components::ComponentConfig &config,
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
                    const auto id = request.GetPathArg("id");

                    int int_id;

                    try {
                        int_id = std::stoi(id);
                    }
                    catch (const std::invalid_argument &e) {
                        auto& response = request.GetHttpResponse();
                        response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
                        return {};
                    }

                    auto result = pg_cluster_->Execute(
                        userver::storages::postgres::ClusterHostType::kMaster,
                        "SELECT id, title, description, organizer_id, is_active, members_limit, start_datetime, finish_datetime, registration_deadline, latitude, longitude, image_url "
                        "FROM cooplan.events "
                        "WHERE is_active = true "
                        "AND id = $1 "
                        "ORDER BY start_datetime",
                        stoi(id)
                    );

                    auto event = result.AsSingleRow<cooplan::Event>(userver::storages::postgres::kRowTag);

                    return userver::formats::json::ToString(userver::formats::json::ValueBuilder{event}.ExtractValue());
                }

            private:
                userver::storages::postgres::ClusterPtr pg_cluster_;

            };

    }  // namespace

    void AppendGetEvent(userver::components::ComponentList &component_list) {
        component_list.Append<GetEvent>();
    }

}  // namespace cooplan
