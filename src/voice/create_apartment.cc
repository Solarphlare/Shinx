#include "create_apartment.h"
#include <dpp/dpp.h>
#include <bsoncxx/builder/stream/document.hpp>
#include "globals/voice_globals.h"
#include "constants.h"
#include "db/voice_interface.h"
#include "db/mongo_instance.h"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

namespace voice {
    dpp::task<void> create_apartment(const dpp::voice_state_update_t& event) {
        dpp::confirmation_callback_t callback = co_await event.owner->co_guild_get_member(event.state.guild_id, event.state.user_id);
        if (callback.is_error()) {
            co_return;
        }

        dpp::guild_member member = callback.get<dpp::guild_member>();

        dpp::channel new_channel = dpp::channel();

        auto& database = db::get_database();
        auto config_db = database["user_config"];

        auto result = config_db.find_one(make_document(kvp("_id", member.user_id.str())));

        if (!result) {
            new_channel.set_name(member.get_user()->global_name + "'s Apartment");
        }
        else {
            auto default_name = result->view().find("default_apartment_name");

            if (default_name == result->view().end()) {
                new_channel.set_name(member.get_user()->global_name + "'s Apartment");
            }
            else {
                new_channel.set_name(std::string(default_name->get_string()));
            }
        }

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

        co_await event.owner->co_message_create(
            dpp::message("Welcome to your apartment. Check out /voice for commands you can use to customize your apartment and manage who can join.").set_channel_id(created_channel.id)
        );
    }
}
