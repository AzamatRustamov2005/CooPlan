#pragma once

#include <chrono>
#include <userver/formats/json/value_builder.hpp>
#include <string>

namespace cooplan {
  struct Event {
    int id;
    std::string title;
    std::string description;
    int organizer_id;
    bool is_active;
    int members_limit;
    std::chrono::system_clock::time_point start_datetime;
    std::chrono::system_clock::time_point finish_datetime;
    std::chrono::system_clock::time_point registration_deadline;
    double latitude;
    double longitude;
    std::string image_url;
  };

  userver::formats::json::Value Serialize(const Event& data,
                                          userver::formats::serialize::To<userver::formats::json::Value>);
}
