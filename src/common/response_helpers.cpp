#include "response_helpers.hpp"
#include <userver/formats/json.hpp>
#include <userver/server/http/http_status.hpp>

namespace cooplan::common {

std::string GetBadResponse(const userver::server::http::HttpRequest &request, const std::string &message) {
    userver::formats::json::ValueBuilder error_response;
    error_response["detail"] = message;

    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
    return userver::formats::json::ToString(error_response.ExtractValue());
}

}  // namespace cooplan::common