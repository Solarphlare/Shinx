#pragma once
#include <dpp/dpp.h>

namespace db::voice {
    // Apartments are stored as a mapping of apartment channel IDs to their owners.
    // The owner is the user who has created the apartment and has permissions to manage it.
    dpp::task<std::unordered_map<dpp::snowflake, dpp::snowflake>> get_apartments(dpp::cluster* bot);
    void add_apartment(const dpp::snowflake& owner_id, const dpp::snowflake& apartment_id, const dpp::snowflake& guild_id);
    void remove_apartment(const dpp::snowflake& apartment_id);
    void transfer_apartment(const dpp::snowflake& apartment_id, const dpp::snowflake& new_owner_id);
}
