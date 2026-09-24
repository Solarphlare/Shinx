#include "commands/voice/limit.h"
#include <dpp/dpp.h>
#include "globals/voice_globals.h"
#include "util/util.h"
#include "util/command_checks.h"

namespace commands::limit {
    dpp::task<void> execute(const dpp::slashcommand_t& event) {
        if (!(co_await util::check_voice_command_requirements(event))) co_return;

        dpp::snowflake current_channel_id = user_locations[event.command.usr.id];

        auto subcommand = event.command.get_command_interaction().options[0];
        const uint8_t new_limit = (uint8_t) subcommand.get_value<int64_t>(0);

        dpp::channel channel = co_await util::get_channel(event.owner, current_channel_id);
        channel.set_user_limit(new_limit);

        dpp::confirmation_callback_t callback = co_await event.owner->co_channel_edit(channel);
        if (callback.is_error()) {
            co_await event.co_reply(dpp::message("Something went wrong while trying to set the user limit for the apartment. Try giving it another shot.").set_flags(dpp::m_ephemeral));
            std::cerr << "Failed to set user limit for apartment: " << callback.get_error().message << "\n";
        }
        else if (new_limit == 0) {
            co_await event.co_reply(dpp::message("The user limit for your apartment has been removed.").set_flags(dpp::m_ephemeral));
        }
        else {
            co_await event.co_reply(dpp::message("The user limit for your apartment has been set to " + std::to_string(new_limit) + ".").set_flags(dpp::m_ephemeral));
        }
    }
}
