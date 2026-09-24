#include "events/voice_state_update.h"
#include <dpp/dpp.h>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <bsoncxx/builder/basic/document.hpp>

#include "constants.h"
#include "globals/voice_globals.h"

#include "voice/create_apartment.h"
#include "voice/handle_user_disconnect.h"

#include "db/voice_interface.h"

namespace events {
    dpp::task<void> handle_voice_state_update(const dpp::voice_state_update_t& event) {
        std::unordered_map<dpp::snowflake, dpp::snowflake> apartments = co_await db::voice::get_apartments(event.owner);

        if (event.state.channel_id == LOBBY_CHANNEL_ID) {
            co_await voice::create_apartment(event);
        }
        else if (event.state.channel_id.empty()) {
            co_await voice::handle_user_disconnect(event);
        }
        else if (apartments.find(event.state.channel_id) != apartments.end()) {
            user_locations[event.state.user_id] = event.state.channel_id;
        }
        else if (user_locations.find(event.state.user_id) != user_locations.end()) {
            // user switched to a channel that isn't an apartment, remove them from the map
            user_locations.erase(event.state.user_id);
        }
    }
}
