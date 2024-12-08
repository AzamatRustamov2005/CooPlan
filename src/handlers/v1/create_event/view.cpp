#include "view.hpp"

#include <fmt/format.h>

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_status.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include <userver/storages/postgres/io/chrono.hpp> // Include for TimePoint types

#include "../../../models/event.hpp"
#include "../../lib/auth.hpp"
#include "../../../common/response_helpers.hpp"

namespace cooplan {

    namespace {

        class CreateEvent final : public userver::server::handlers::HttpHandlerBase {
            public:
                static constexpr std::string_view
                kName = "handler-create-event";

                CreateEvent(const userver::components::ComponentConfig &config,
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
                    const auto session = GetSessionInfo(pg_cluster_, request);
                    if (!session) {
                        auto& response = request.GetHttpResponse();
                        response.SetStatus(userver::server::http::HttpStatus::kUnauthorized);
                        return {};
                    }

                    try {
                        // Parse request body
                        auto request_body = userver::formats::json::FromString(request.RequestBody());

                        // Extract fields from request body
                        const auto title = request_body["title"].As<std::optional<std::string>>();
                        const auto description = request_body["description"].As<std::optional<std::string>>("");
                        const auto members_limit = request_body["members_limit"].As<std::optional<int>>(std::nullopt);
                        const auto start_datetime = request_body["start_datetime"].As<std::optional<std::chrono::system_clock::time_point>>(std::nullopt);
                        const auto finish_datetime = request_body["finish_datetime"].As<std::optional<std::chrono::system_clock::time_point>>(std::nullopt);
                        const auto latitude = request_body["latitude"].As<std::optional<double>>(std::nullopt);
                        const auto longitude = request_body["longitude"].As<std::optional<double>>(std::nullopt);
                        const auto image_url = request_body["image_url"].As<std::optional<std::string>>();

                        // Validate mandatory fields
                        if (!title) {
                            return cooplan::common::GetBadResponse(request, "'title' field is required");
                        }

                        if (!image_url) {
                            return cooplan::common::GetBadResponse(request, "'image_url' field is required");
                        }

                        const auto query = R"SQL(
                            INSERT INTO cooplan.events (
                                title, description, organizer_id, is_active,
                                members_limit, start_datetime, finish_datetime, latitude,
                                longitude, image_url
                            ) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)
                            RETURNING *
                        )SQL";

                        auto result = pg_cluster_->Execute(
                            userver::storages::postgres::ClusterHostType::kMaster, // Specify master host for write operations
                            userver::storages::postgres::Query(query),
                            *title, description.value_or(""), session->user_id, true,
                            members_limit.value_or(0),
                            start_datetime ? userver::storages::postgres::TimePointWithoutTz{*start_datetime}
                                            : userver::storages::postgres::TimePointWithoutTz{}, // Convert start_datetime
                            finish_datetime ? userver::storages::postgres::TimePointWithoutTz{*finish_datetime}
                                            : userver::storages::postgres::TimePointWithoutTz{}, // Convert finish_datetime
                            latitude.value_or(0.0), 
                            longitude.value_or(0.0), 
                            *image_url
                        );

                        // Map the result to the Event model
                        auto event = result.AsSingleRow<cooplan::Event>(userver::storages::postgres::kRowTag);

                        // Return the serialized event as JSON
                        return userver::formats::json::ToString(userver::formats::json::ValueBuilder{event}.ExtractValue());

                    } catch (const userver::formats::json::Exception &e) {
                        auto &response = request.GetHttpResponse();
                        response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
                        return fmt::format("{{\"error\":\"Invalid JSON: {}\"}}", e.what());

                    } catch (const userver::storages::postgres::Error &e) {
                        auto &response = request.GetHttpResponse();
                        response.SetStatus(userver::server::http::HttpStatus::kInternalServerError);
                        return fmt::format("{{\"error\":\"Database error: {}\"}}", e.what());
                    }
                }

            private:
                userver::storages::postgres::ClusterPtr pg_cluster_;

            };

    }  // namespace

    void AppendCreateEvent(userver::components::ComponentList &component_list) {
        component_list.Append<CreateEvent>();
    }

}  // namespace cooplan
