#include "voice/handle_user_disconnect.h"
#include <dpp/dpp.h>
#include "globals/voice_globals.h"
#include "db/voice_interface.h"

namespace voice {
    dpp::task<void> handle_user_disconnect(const dpp::voice_state_update_t& event) {
        auto it = user_locations.find(event.state.user_id);
        if (it == user_locations.end()) {
            co_return; // user was not in an apartment
        }

        dpp::snowflake apartment_id = it->second;
        user_locations.erase(it);

        dpp::confirmation_callback_t get_channel_callback = co_await event.owner->co_channel_get(apartment_id);
        if (get_channel_callback.is_error()) {
            std::cerr << "Failed to get channel: " << get_channel_callback.get_error().human_readable << "\n";
            co_return;
        }

        dpp::channel apartment_channel = get_channel_callback.get<dpp::channel>();
        auto members = apartment_channel.get_voice_members();

        if (members.empty()) {
            dpp::confirmation_callback_t delete_callback = co_await event.owner->co_channel_delete(apartment_id);
            if (delete_callback.is_error()) {
                std::cerr << "Failed to delete empty apartment: " << delete_callback.get_error().human_readable << "\n";
            }
            else {
                db::voice::remove_apartment(apartment_id);
            }
        }
    }
}
