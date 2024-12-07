#pragma once

#include <string>
#include <userver/server/http/http_request.hpp>

namespace cooplan::common {

std::string GetBadResponse(const userver::server::http::HttpRequest &request, const std::string &message);

}  // namespace cooplan::common