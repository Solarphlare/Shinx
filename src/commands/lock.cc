#include "lock.h"
#include <dpp/dpp.h>
#include <algorithm>
#include <iostream>
#include "globals/voice_globals.h"
#include "util/command_checks.h"

namespace commands::lock {
    dpp::task<void> execute(const dpp::slashcommand_t& event) {
        if (!(co_await util::check_voice_command_requirements(event))) co_return;
        
        dpp::snowflake current_channel_id = user_locations[event.command.usr.id];

        dpp::confirmation_callback_t callback = co_await event.owner->co_channel_get(current_channel_id);
        if (callback.is_error()) {
            co_await event.co_reply(dpp::message("Something went wrong. Try giving it another shot.").set_flags(dpp::m_ephemeral));
            co_return;
        }

        dpp::channel channel = callback.get<dpp::channel>();
        dpp::permission_overwrite overwrite = *std::find_if(channel.permission_overwrites.begin(), channel.permission_overwrites.end(),
            [&event](const dpp::permission_overwrite& o) { return o.type == dpp::ot_role && o.id == event.command.guild_id; });

        if (overwrite.deny & dpp::p_connect) {
            std::cout << overwrite.deny << "\n";
            co_await event.co_reply(dpp::message("Your apartment is already locked.").set_flags(dpp::m_ephemeral));
            co_return;
        }

        channel.set_permission_overwrite(event.command.guild_id, dpp::ot_role, 0, dpp::p_connect);
        dpp::confirmation_callback_t update_callback = co_await event.owner->co_channel_edit(channel);
        if (update_callback.is_error()) {
            std::cerr << "Failed to lock the apartment: " << update_callback.get_error().message << "\n";
            co_await event.co_reply(dpp::message("Failed to lock the apartment.").set_flags(dpp::m_ephemeral));
            co_return;
        }

        co_await event.co_reply(dpp::message("Your apartment has been locked.").set_flags(dpp::m_ephemeral));
    }
}
