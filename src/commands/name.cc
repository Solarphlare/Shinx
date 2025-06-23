#include "name.h"
#include <dpp/dpp.h>
#include "globals/voice_globals.h"
#include "util/util.h"
#include "util/command_checks.h"

namespace commands::name {
    dpp::task<void> execute(const dpp::slashcommand_t& event) {
        if (!(co_await util::check_voice_command_requirements(event))) co_return;

        dpp::snowflake current_channel_id = user_locations[event.command.usr.id];

        auto subcommand = event.command.get_command_interaction().options[0];
        std::string new_name = subcommand.get_value<std::string>(0);

        dpp::channel channel = co_await util::get_channel(event.owner, current_channel_id);

        channel.set_name(new_name);

        dpp::confirmation_callback_t callback = co_await event.owner->co_channel_edit(channel);
        if (callback.is_error()) {
            co_await event.co_reply(dpp::message("Failed to rename the apartment.").set_flags(dpp::m_ephemeral));
            std::cerr << "Failed to rename apartment: " << callback.get_error().message << "\n";
        }
        else {
            co_await event.co_reply(dpp::message("Your apartment has been renamed to " + new_name + ".").set_flags(dpp::m_ephemeral));
        }
    }
}
