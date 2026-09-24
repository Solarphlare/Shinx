#include "util/util.h"
#include <dpp/dpp.h>

namespace util {
    dpp::task<dpp::guild_member> get_guild_member(dpp::cluster* bot, const dpp::snowflake guild_id, const dpp::snowflake user_id) {
        try {
            dpp::guild_member guild_member = dpp::find_guild_member(guild_id, user_id);
            co_return guild_member;
        }
        catch (...) {}

        dpp::confirmation_callback_t callback = co_await bot->co_guild_get_member(guild_id, user_id);
        if (callback.is_error()) {
            throw std::runtime_error("Failed to get guild member: " + callback.get_error().message);
        }

        co_return callback.get<dpp::guild_member>();
    }

    dpp::task<dpp::guild> get_guild(dpp::cluster* bot, const dpp::snowflake guild_id) {
        dpp::guild* guild = dpp::find_guild(guild_id);
        if (guild != nullptr) co_return *guild;

        dpp::confirmation_callback_t callback = co_await bot->co_guild_get(guild_id);
        if (callback.is_error()) {
            throw std::runtime_error("Failed to get guild: " + callback.get_error().message);
        }

        co_return callback.get<dpp::guild>();
    }

    dpp::task<dpp::channel> get_channel(dpp::cluster* bot, const dpp::snowflake channel_id) {
        dpp::channel* channel = dpp::find_channel(channel_id);
        if (channel != nullptr) co_return *channel;

        dpp::confirmation_callback_t callback = co_await bot->co_channel_get(channel_id);
        if (callback.is_error()) {
            throw std::runtime_error("Failed to get channel: " + callback.get_error().message);
        }

        co_return callback.get<dpp::channel>();
    }
}

