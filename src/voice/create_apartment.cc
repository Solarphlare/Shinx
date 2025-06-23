#include "create_apartment.h"
#include <dpp/dpp.h>
#include "globals/voice_globals.h"
#include "constants.h"
#include "db/voice_interface.h"

namespace voice {
    dpp::task<void> create_apartment(const dpp::voice_state_update_t& event) {
        dpp::confirmation_callback_t callback = co_await event.owner->co_guild_get_member(event.state.guild_id, event.state.user_id);
        if (callback.is_error()) {
            co_return;
        }

        dpp::guild_member member = callback.get<dpp::guild_member>();

        dpp::channel new_channel = dpp::channel();
        new_channel.set_name(member.get_user()->global_name + "'s Apartment");
        new_channel.set_parent_id(VOICE_CHANNEL_CATEGORY_ID);
        new_channel.set_type(dpp::channel_type::CHANNEL_VOICE);
        new_channel.set_guild_id(event.state.guild_id);

        // need this to allow the bot to manage the channel (denying connect will deny manage channels. why???)
        new_channel.add_permission_overwrite(event.owner->me.id, dpp::ot_member, dpp::p_connect, 0);

        // locking will not work without this. i don't know why.
        new_channel.remove_permission_overwrite(event.state.guild_id, dpp::ot_role);

        dpp::confirmation_callback_t create_channel_callback = co_await event.owner->co_channel_create(new_channel);
        if (create_channel_callback.is_error()) {
            std::cerr << "Failed to create apartment channel: " << create_channel_callback.get_error().human_readable << "\n";
            co_await event.owner->co_guild_member_move(0, event.state.guild_id, event.state.user_id); // disconnect user
        }

        dpp::channel created_channel = create_channel_callback.get<dpp::channel>();
        dpp::confirmation_callback_t move_callback = co_await event.owner->co_guild_member_move(created_channel.id, event.state.guild_id, event.state.user_id);
        if (move_callback.is_error()) {
            std::cerr << "Failed to move user to new channel: " << move_callback.get_error().human_readable << "\n";
            co_await event.owner->co_channel_delete(created_channel.id); // delete the channel if moving failed
            co_await event.owner->co_guild_member_move(0, event.state.guild_id, event.state.user_id); // disconnect user
            co_return;
        }

        db::voice::add_apartment(event.state.user_id, created_channel.id, event.state.guild_id);
        user_locations[event.state.user_id] = created_channel.id;

        // todo: send kickoff message
    }
}
