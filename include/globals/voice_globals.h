#pragma once
#include <unordered_map>
#include <dpp/dpp.h>

// User locations map keyed by guild and then user ID to track which user is in which voice channel (apartment).
// Users who are in a voice channel that is not an apartment will not have an entry in this map.
extern std::unordered_map<dpp::snowflake, std::unordered_map<dpp::snowflake, dpp::snowflake>> user_locations;
