#pragma once
#include <dpp/dpp.h>

namespace util {
    dpp::task<dpp::guild_member> get_guild_member(dpp::cluster* bot, const dpp::snowflake guild_id, const dpp::snowflake user_id);
    dpp::task<dpp::guild> get_guild(dpp::cluster* bot, const dpp::snowflake guild_id);
    dpp::task<dpp::channel> get_channel(dpp::cluster* bot, const dpp::snowflake channel_id);
    dpp::task<dpp::user_identified> get_user(dpp::cluster* bot, const dpp::snowflake& user_id);
    dpp::task<dpp::guild_map> get_bot_guilds(dpp::cluster* bot);
    dpp::task<dpp::snowflake> get_bot_owner(dpp::cluster* bot);
}
