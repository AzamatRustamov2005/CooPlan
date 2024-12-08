#include "event_participant.hpp"

namespace cooplan {
userver::formats::json::Value Serialize(const event_participant& event_participant,
                                        userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder json_obj;

  json_obj["id"] = event_participant.id;
  json_obj["status"] = event_participant.status;
  json_obj["user_id"] = event_participant.user_id;
  json_obj["event_id"] = event_participant.event_id;

  return json_obj.ExtractValue();
}
}