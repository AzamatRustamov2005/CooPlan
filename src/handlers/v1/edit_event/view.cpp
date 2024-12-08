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

    class EditEvent final : public userver::server::handlers::HttpHandlerBase {
    public:
        static constexpr std::string_view kName = "handler-edit-event";

        EditEvent(const userver::components::ComponentConfig &config,
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

            const auto event_id_str = request.GetPathArg("id");
            if (event_id_str.empty()) {
                request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
                return BadResponse(request, "Event ID is required");
            }

            int event_id;
            try {
                event_id = std::stoi(event_id_str);
            } catch (const std::exception&) {
                request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
                return BadResponse(request, "Invalid Event ID format");
            }

            auto result = pg_cluster_->Execute(
                userver::storages::postgres::ClusterHostType::kMaster,
                "SELECT organizer_id FROM cooplan.events WHERE id = $1",
                event_id);

            if (result.IsEmpty()) {
                request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
                return BadResponse(request, "Event not found");
            }

            const auto organizer_id = result.AsSingleRow<int>();
            if (organizer_id != user_id) {
                request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
                return BadResponse(request, "Unauthorized: User is not the organizer of this event");
            }

            const auto request_body = userver::formats::json::FromString(request.RequestBody());
            std::vector<std::string> updates;
            if (request_body.HasMember("title")) {
                updates.emplace_back(fmt::format("title = '{}'", request_body["title"].As<std::string>()));
            }
            if (request_body.HasMember("description")) {
                updates.emplace_back(fmt::format("description = '{}'", request_body["description"].As<std::string>()));
            }
            if (request_body.HasMember("is_active")) {
                updates.emplace_back(fmt::format("is_active = {}", request_body["is_active"].As<bool>()));
            }
            if (request_body.HasMember("members_limit")) {
                updates.emplace_back(fmt::format("members_limit = {}", request_body["members_limit"].As<int>()));
            }
            if (request_body.HasMember("start_datetime")) {
                updates.emplace_back(fmt::format("start_datetime = '{}'", request_body["start_datetime"].As<std::string>()));
            }
            if (request_body.HasMember("finish_datetime")) {
                updates.emplace_back(fmt::format("finish_datetime = '{}'", request_body["finish_datetime"].As<std::string>()));
            }
            if (request_body.HasMember("registration_deadline")) {
                updates.emplace_back(fmt::format("registration_deadline = '{}'", request_body["registration_deadline"].As<std::string>()));
            }
            if (request_body.HasMember("latitude")) {
                updates.emplace_back(fmt::format("latitude = {}", request_body["latitude"].As<double>()));
            }
            if (request_body.HasMember("longitude")) {
                updates.emplace_back(fmt::format("longitude = {}", request_body["longitude"].As<double>()));
            }
            if (request_body.HasMember("image_url")) {
                updates.emplace_back(fmt::format("image_url = '{}'", request_body["image_url"].As<std::string>()));
            }

            auto update_query = fmt::format("UPDATE cooplan.events SET {} WHERE id = {}", fmt::join(updates, ", "), event_id);
            pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, update_query);

            userver::formats::json::ValueBuilder response;
            response["status"] = "updated";
            return userver::formats::json::ToString(response.ExtractValue());
        }

    private:
        userver::storages::postgres::ClusterPtr pg_cluster_;
    };

}  // namespace

void AppendEditEvent(userver::components::ComponentList &component_list) {
    component_list.Append<EditEvent>();
}

}  // namespace cooplan