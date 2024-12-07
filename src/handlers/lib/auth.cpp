#include "auth.hpp"

namespace cooplan {

std::optional<TSession> GetSessionInfo(
    userver::storages::postgres::ClusterPtr pg_cluster,
    const userver::server::http::HttpRequest& request
) {
    if (!request.HasHeader(USER_TICKET_HEADER_NAME)) {
        return std::nullopt;
    }

    auto id = request.GetHeader(USER_TICKET_HEADER_NAME);
    auto result = pg_cluster->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT * FROM cooplan.auth_sessions "
        "WHERE id = $1",
        id
    );

    if (result.IsEmpty()) {
        return std::nullopt;
    }

    return result.AsSingleRow<TSession>(userver::storages::postgres::kRowTag);
}

std::optional<std::string> CreateSession(
    userver::storages::postgres::ClusterPtr pg_cluster, int user_id
) {
    try {
        auto result = pg_cluster->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            "INSERT INTO cooplan.auth_sessions(user_id) VALUES($1) "
            "ON CONFLICT DO NOTHING "
            "RETURNING auth_sessions.id",
            user_id
        );

        if (result.IsEmpty()) {
            return std::nullopt;
        }

        return result.AsSingleRow<std::string>();
    } catch (const std::exception& e) {
        // Handle exceptions appropriately
        return std::nullopt;
    }
}

}  // namespace cooplan