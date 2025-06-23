#include "permit.h"
#include <dpp/dpp.h>
#include <format>
#include "util/command_checks.h"
#include "util/util.h"
#include "globals/voice_globals.h"

namespace commands::permit {
    dpp::task<void> execute(const dpp::slashcommand_t& event) {
        dpp::snowflake current_channel_id = user_locations[event.command.usr.id];
        if (!(co_await util::check_voice_command_requirements(event, current_channel_id))) co_return;

        auto subcommand = event.command.get_command_interaction().options[0];
        dpp::user target_user = event.command.get_resolved_user(subcommand.get_value<dpp::snowflake>(0));

        dpp::channel channel = co_await util::get_channel(event.owner, current_channel_id);
        channel.add_permission_overwrite(target_user.id, dpp::ot_member, dpp::p_connect, 0);

        dpp::confirmation_callback_t callback = co_await event.owner->co_channel_edit(channel);
        if (callback.is_error()) {
            co_await event.co_reply(dpp::message("Something went wrong. Try giving it another shot.").set_flags(dpp::m_ephemeral));
            co_return;
        }

        co_await event.co_reply(dpp::message(std::format("{} has been granted permission to join your apartment.", target_user.get_mention())));
    }
}
