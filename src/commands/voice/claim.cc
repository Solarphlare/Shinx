#include "commands/voice/claim.h"
#include <dpp/dpp.h>
#include <iostream>
#include <format>
#include "util/util.h"
#include "db/voice_interface.h"
#include "globals/voice_globals.h"

namespace commands::claim {
    dpp::task<void> execute(const dpp::slashcommand_t& event) {
        if (user_locations.find(event.command.usr.id) == user_locations.end()) {
            co_await event.co_reply(dpp::message("You need to be in an apartment to use this.").set_flags(dpp::m_ephemeral));
            co_return;
        }

        dpp::snowflake current_channel_id = user_locations[event.command.usr.id];

        auto apartments = co_await db::voice::get_apartments(event.owner);
        dpp::snowflake apartment_owner_id = apartments[current_channel_id];

        if (apartment_owner_id == event.command.usr.id) {
            co_await event.co_reply(dpp::message("You already own this apartment.").set_flags(dpp::m_ephemeral));
            co_return;
        }

        auto owner_location = user_locations.find(apartment_owner_id);

        if (owner_location != user_locations.end() && owner_location->second == current_channel_id) {
            co_await event.co_reply(dpp::message(std::format("You can only claim this apartment after the owner (<@{}>) has left.", apartment_owner_id.str())).set_flags(dpp::m_ephemeral));
            co_return;
        }

        db::voice::transfer_apartment(current_channel_id, event.command.usr.id);
        co_await event.co_reply(dpp::message("You have successfully claimed this apartment!").set_flags(dpp::m_ephemeral));
    }
}
