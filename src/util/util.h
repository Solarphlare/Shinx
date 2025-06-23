#pragma once
#include <dpp/dpp.h>

namespace util {
    dpp::task<dpp::guild_member> get_guild_member(dpp::cluster* bot, const dpp::snowflake guild_id, const dpp::snowflake user_id);
    dpp::task<dpp::guild> get_guild(dpp::cluster* bot, const dpp::snowflake guild_id);
    dpp::task<dpp::channel> get_channel(dpp::cluster* bot, const dpp::snowflake channel_id);
}
