#pragma once

#include <userver/server/http/http_request.hpp>
#include <userver/storages/postgres/cluster.hpp>

#include "../../models/session.hpp"

namespace cooplan {

const std::string USER_TICKET_HEADER_NAME = "Authorization";

std::optional<TSession> GetSessionInfo(
    userver::storages::postgres::ClusterPtr pg_cluster,
    const userver::server::http::HttpRequest& request
);

std::optional<std::string> CreateSession(
    userver::storages::postgres::ClusterPtr pg_cluster, int user_id
);

}  // namespace cooplan