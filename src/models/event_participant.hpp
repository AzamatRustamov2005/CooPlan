#pragma once

#include <chrono>
#include <userver/formats/json/value_builder.hpp>
#include <string>

namespace cooplan {
struct event_participant {
  int id;
  std::string status;
  int event_id;
  int user_id;
};

userver::formats::json::Value Serialize(const event_participant& data,
                                        userver::formats::serialize::To<userver::formats::json::Value>);
}