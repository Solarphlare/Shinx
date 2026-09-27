#include "events/voice_state_update.h"
#include <dpp/dpp.h>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <bsoncxx/builder/basic/document.hpp>

#include "globals/voice_globals.h"

#include "voice/create_apartment.h"
#include "voice/handle_user_disconnect.h"

#include "db/voice_interface.h"
#include "db/mongo_instance.h"
#include "util/util.h"

namespace events {
    dpp::task<void> handle_voice_state_update(const dpp::voice_state_update_t& event) {
        std::unordered_map<dpp::snowflake, dpp::snowflake> apartments = co_await db::voice::get_apartments(event.owner, event.state.guild_id);

        auto settings_doc = db::get_database(event.state.guild_id).collection("misc").find_one(
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("type", "settings")
            )
        );

        if (!settings_doc || !settings_doc->view()["lobby_channel_id"]) co_return;
        const dpp::snowflake lobby_channel_id = static_cast<std::string>(settings_doc->view()["lobby_channel_id"].get_string().value);

        if (event.state.channel_id == lobby_channel_id) {
            co_await voice::create_apartment(event);
        }
        else if (event.state.channel_id.empty()) {
            co_await voice::handle_user_disconnect(event);
        }
        else if (apartments.find(event.state.channel_id) != apartments.end()) {
            user_locations[event.state.guild_id][event.state.user_id] = event.state.channel_id;
        }
        else if (user_locations[event.state.guild_id].find(event.state.user_id) != user_locations[event.state.guild_id].end()) {
            // user switched to a channel that isn't an apartment, remove them from the map
            user_locations[event.state.guild_id].erase(event.state.user_id);
        }
    }
}
