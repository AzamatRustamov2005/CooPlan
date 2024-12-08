#pragma once

#include <userver/components/component_list.hpp>

namespace cooplan{

// Registers the handler for retrieving user profiles
void AppendGetProfile(userver::components::ComponentList& component_list);

}  // namespace cooplan::handlers::v1::get_profile
