#include "view.hpp"

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <string>

#include "../../../models/event.hpp"

namespace cooplan {

namespace {

// Helper function to handle error responses
std::string GetErrorResponse(const userver::server::http::HttpRequest& request, const std::string& message) {
    userver::formats::json::ValueBuilder error_response;
    error_response["message"] = message;

    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
    return userver::formats::json::ToString(error_response.ExtractValue());
}

class GetProfile final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-get-profile";

    GetProfile(const userver::components::ComponentConfig& config,
               const userver::components::ComponentContext& component_context)
        : HttpHandlerBase(config, component_context),
          pg_cluster_(
              component_context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster()) {}

    std::string HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                   userver::server::request::RequestContext&) const override {

        const auto user_id = request.GetPathArg("id");

        if (user_id.empty()) {
            return GetErrorResponse(request, "User not found.");
        }

        auto result = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            "SELECT username, id, contact, image "
            "FROM cooplan.users WHERE id = CAST($1 AS INTEGER)",
            user_id);

        if (result.IsEmpty()) {
            return GetErrorResponse(request, "User not found.");
        }

        userver::formats::json::ValueBuilder response;

        response["username"] = userver::formats::json::ValueBuilder(
            result[0]["username"].As<std::string>()).ExtractValue();
        response["contact"] = userver::formats::json::ValueBuilder(
            result[0]["contact"].As<std::string>()).ExtractValue();
        response["image"] = userver::formats::json::ValueBuilder(
            result[0]["image"].As<std::string>()).ExtractValue();

        // Convert the response to a JSON string and return
        return userver::formats::json::ToString(response.ExtractValue());
    }

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

// Function to append the handler to the component list
void AppendGetProfile(userver::components::ComponentList& component_list) {
    component_list.Append<GetProfile>();
}

}  // namespace cooplan::handlers::v1::get_profile
