#pragma once
#include <dpp/dpp.h>

namespace db::voice {
    dpp::task<std::unordered_map<dpp::snowflake, dpp::snowflake>> get_apartments(dpp::cluster* bot);
    void add_apartment(const dpp::snowflake& owner_id, const dpp::snowflake& apartment_id, const dpp::snowflake& guild_id);
    void remove_apartment(const dpp::snowflake& apartment_id);
}
