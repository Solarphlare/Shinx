#include "deny.h"
#include <dpp/dpp.h>
#include <format>
#include <algorithm>
#include "util/command_checks.h"
#include "util/util.h"
#include "globals/voice_globals.h"

namespace commands::deny {
    dpp::task<void> execute(const dpp::slashcommand_t& event) {
        if (!(co_await util::check_voice_command_requirements(event))) co_return;

        dpp::snowflake current_channel_id = user_locations[event.command.usr.id];

        auto subcommand = event.command.get_command_interaction().options[0];
        dpp::user target_user = event.command.get_resolved_user(subcommand.get_value<dpp::snowflake>(0));

        if (target_user.is_bot()) {
            co_await event.co_reply(dpp::message("You can't deny access to a bot.").set_flags(dpp::m_ephemeral));
            co_return;
        }

        if (target_user.id == event.command.usr.id) {
            co_await event.co_reply(dpp::message("You can't deny yourself access to an apartment that you own!").set_flags(dpp::m_ephemeral));
            co_return;
        }

        dpp::channel channel = co_await util::get_channel(event.owner, current_channel_id);

        auto channel_permissions = channel.permission_overwrites;
        auto target_user_overwrite_it = std::find_if(channel_permissions.begin(), channel_permissions.end(),
            [&target_user](const dpp::permission_overwrite& overwrite) {
                return overwrite.id == target_user.id && overwrite.type == dpp::ot_member;
            });

        if (target_user_overwrite_it != channel_permissions.end() && target_user_overwrite_it->deny & dpp::p_connect) {
            co_await event.co_reply(dpp::message(std::format("{} already doesn't have access to your apartment.", target_user.get_mention())).set_flags(dpp::m_ephemeral));
            co_return;
        }

        channel.add_permission_overwrite(target_user.id, dpp::ot_member, 0, dpp::p_connect);

        dpp::confirmation_callback_t callback = co_await event.owner->co_channel_edit(channel);
        if (callback.is_error()) {
            co_await event.co_reply(dpp::message("Something went wrong. Try giving it another shot.").set_flags(dpp::m_ephemeral));
            co_return;
        }

        auto target_user_location = user_locations.find(target_user.id);

        if (target_user_location != user_locations.end() && target_user_location->second == current_channel_id) {
            co_await event.owner->co_guild_member_move(0, event.command.guild_id, target_user.id);
        }

        co_await event.co_reply(dpp::message(std::format("{} has been denied access to your apartment.", target_user.get_mention())).set_flags(dpp::m_ephemeral));
    }
}
