#include "event.hpp"

namespace cooplan {
  userver::formats::json::Value Serialize(const Event& event,
                                          userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder json_obj;

    // Setting each key-value pair
    json_obj["id"] = event.id;
    json_obj["title"] = event.title;
    json_obj["description"] = event.description;
    json_obj["organizer_id"] = event.organizer_id;
    json_obj["is_active"] = event.is_active;
    json_obj["members_limit"] = event.members_limit;
    json_obj["start_datetime"] = event.start_datetime;
    json_obj["finish_datetime"] = event.finish_datetime;
    json_obj["registration_deadline"] = event.registration_deadline;
    json_obj["latitude"] = event.latitude,
    json_obj["longitude"] = event.longitude,
    json_obj["image_url"] = event.image_url;

    return json_obj.ExtractValue();
  }
}