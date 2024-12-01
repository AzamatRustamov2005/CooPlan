#include "view.hpp"

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
        std::string GetBadResponse(const userver::server::http::HttpRequest &request, const std::string& message) {
            userver::formats::json::ValueBuilder error_response;
            error_response["message"] = message;

            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
            return userver::formats::json::ToString(error_response.ExtractValue());
        }

        class RegisterUser final : public userver::server::handlers::HttpHandlerBase {
        public:
            static constexpr std::string_view
            kName = "handler-register-user";

            RegisterUser(const userver::components::ComponentConfig &config,
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
                const auto username = request.GetFormDataArg("username").value;
                const auto password = request.GetFormDataArg("password").value;
                // напиши поля в запросе

                if (username.empty() || password.empty()) {
                    return GetBadResponse(request, "username or password cannot be empty");
                }

                const auto password_encrypted = userver::crypto::hash::Sha256(password);

                auto result = pg_cluster_->Execute(
                        userver::storages::postgres::ClusterHostType::kMaster,
                        "INSERT INTO cooplan.users(username, password) VALUES($1, $2) "
                        "ON CONFLICT DO NOTHING "
                        "RETURNING users.id",
                        username, password_encrypted
                );

                if (result.IsEmpty()) {
                    return GetBadResponse(request, "User with such username already exists");
                }

                return {};
            }

        private:
            userver::storages::postgres::ClusterPtr pg_cluster_;
        };

    }  // namespace

    void AppendRegisterUser(userver::components::ComponentList &component_list) {
        component_list.Append<RegisterUser>();
    }

}  // namespace cooplan
