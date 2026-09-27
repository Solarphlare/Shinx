#include "util/command_checks.h"
#include <dpp/dpp.h>
#include "db/voice_interface.h"
#include "globals/voice_globals.h"
#include "util/util.h"

namespace util {
    dpp::task<bool> check_voice_command_requirements(const dpp::slashcommand_t& event) {
        auto current_channel_id_it = user_locations[event.command.guild_id].find(event.command.usr.id);
        if (current_channel_id_it == user_locations[event.command.guild_id].end()) {
            event.reply(dpp::message("You need to be in an apartment that you own to use this.").set_flags(dpp::m_ephemeral), [](const dpp::confirmation_callback_t& callback) {});
            co_return false;
        }

        dpp::snowflake current_channel_owner_uid = (co_await db::voice::get_apartments(event.owner, event.command.guild_id))[current_channel_id_it->second];

        if (current_channel_owner_uid != event.command.usr.id) {
            event.reply(dpp::message("Only the owner of the apartment can use this.").set_flags(dpp::m_ephemeral), [](const dpp::confirmation_callback_t& callback) {});
            co_return false;
        }

        co_return true;
    }
}
