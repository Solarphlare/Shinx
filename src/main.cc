#include <dpp/dpp.h>
#include <iostream>

#include "config.h"
#include "events/voice_state_update.h"
#include "events/slash_command_handler.h"
#include "util/voice_state_cache.h"

#include "commands/lock.h"
#include "commands/unlock.h"

static dpp::cluster bot{BOT_TOKEN};

int main() {
    bot.intents = dpp::i_default_intents | dpp::i_guild_voice_states | dpp::i_guild_members;

    #ifdef DEBUG
    std::cout << "====== DEBUG BUILD ======\n";
    #endif

    bot.on_log(dpp::utility::cout_logger());

    bot.on_ready([](const dpp::ready_t &event) -> dpp::task<void> {
        std::cout << "Logged in as " << bot.me.format_username() << "\n";
        co_await util::build_voice_state_cache(&bot);
    });

    bot.on_voice_state_update([](const dpp::voice_state_update_t &event) -> dpp::task<void> {
        co_await events::handle_voice_state_update(event);
    });

    bot.on_slashcommand([](const dpp::slashcommand_t &event) -> dpp::task<void> {
        co_await events::handle_slash_command(event);
    });

    bot.start();
}
