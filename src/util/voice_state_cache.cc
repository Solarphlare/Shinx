#include "util/voice_state_cache.h"
#include <dpp/dpp.h>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <utility>
#include "db/voice_interface.h"

#include "globals/voice_globals.h"

namespace util {
    dpp::task<void> build_voice_state_cache(dpp::cluster* bot) {
        const auto apartments = co_await db::voice::get_apartments(bot);

        std::unordered_map<dpp::snowflake, dpp::snowflake> new_user_locations;

        for (const auto& [apartment_id, owner_id] : apartments) {
            dpp::confirmation_callback_t callback = co_await bot->co_channel_get(apartment_id);
            if (callback.is_error()) {
                std::cerr << "Failed to get channel for apartment " << apartment_id << ": " << callback.get_error().human_readable << "\n";
                continue; // Skip this apartment if the channel cannot be retrieved
            }

            dpp::channel channel = callback.get<dpp::channel>();
            for (const auto& voice_map : channel.get_voice_members()) {
                new_user_locations[voice_map.first] = apartment_id;
            }
        }

        user_locations = new_user_locations;
        std::cout << "Voice state cache built successfully, cached " << user_locations.size() << " user(s).\n";
    }
}
