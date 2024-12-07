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

        class LoginUser final : public userver::server::handlers::HttpHandlerBase {
        public:
            static constexpr std::string_view
            kName = "handler-login-user";

            LoginUser(const userver::components::ComponentConfig &config,
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
                const auto username = request_body["username"].As<std::optional<std::string>>();
                const auto password = request_body["password"].As<std::optional<std::string>>();

                if (!username.has_value() || username->empty()) {
                    return common::GetBadResponse(request, "Username cannot be empty");
                }

                if (!password.has_value() || password->empty()) {
                    return common::GetBadResponse(request, "Password cannot be empty");
                }

                const auto password_encrypted = userver::crypto::hash::Sha256(password.value());

                auto result = pg_cluster_->Execute(
                        userver::storages::postgres::ClusterHostType::kMaster,
                        "SELECT * FROM cooplan.users "
                        "WHERE username = $1, password = $2",
                        username, password_encrypted
                );

                if (result.IsEmpty()) {
                    return common::GetBadResponse(request, "User with such username not found");
                    /*
                    auto &response = request.GetHttpResponse();
                    response.SetStatus(userver::server::http::HttpStatus::kNotFound);
                    return {};
                    */
                }

                auto user = result.AsSingleRow<User>(userver::storages::postgres::kRowTag);
                if (password != user.password) {
                    return common::GetBadResponse(request, "User password is incorrect");
                    /*
                    auto &response = request.GetHttpResponse();
                    response.SetStatus(userver::server::http::HttpStatus::kNotFound);
                    return {};
                    */
                }

                auto session_id = CreateSession(pg_cluster_, user.id);
                if (!session_id) {
                    return cooplan::common::GetBadResponse(request, "Failed to create session");
                }

                // Success response with user and session data
                userver::formats::json::ValueBuilder response;
                // response["user_id"] = user.id;
                response["token"] = *session_id;

                return userver::formats::json::ToString(response.ExtractValue());
            }

        private:
            userver::storages::postgres::ClusterPtr pg_cluster_;
        };

    }  // namespace

    void AppendLoginUser(userver::components::ComponentList &component_list) {
        component_list.Append<LoginUser>();
    }

}  // namespace cooplan
