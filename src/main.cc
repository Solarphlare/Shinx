#include <dpp/dpp.h>
#include <iostream>
#include <cstdlib>

#include "events/voice_state_update.h"
#include "events/slash_command_handler.h"
#include "util/voice_state_cache.h"
#include "util/create_commands.h"
#include "constants.h"

#include "commands/voice/lock.h"
#include "commands/voice/unlock.h"

static dpp::cluster bot{std::getenv("BOT_TOKEN")};
bool caught_signal = false;

int main(int argc, char** argv) {
    bot.intents = dpp::i_default_intents | dpp::i_guild_voice_states;

    #ifdef DEBUG
    std::cout << "====== DEBUG BUILD ======\n";
    std::cout << "Lobby channel ID: " << LOBBY_CHANNEL_ID << '\n';
    std::cout << "Category ID: " << VOICE_CHANNEL_CATEGORY_ID << '\n';
    #endif

    bot.on_log(dpp::utility::cout_logger());

    bot.on_ready([&argc, &argv](const dpp::ready_t &event) -> dpp::task<void> {
        std::cout << "Logged in as " << bot.me.format_username() << "\n";

        std::signal(SIGINT, [](int signal) {
            if (caught_signal) std::exit(128 + signal);
            else caught_signal = true;

            std::cout << "\nCaught SIGINT, shutting down...\n";
            bot.shutdown();
            std::exit(128 + signal);
        });

        if (argc > 1 && std::string(argv[1]) == "--register-commands") {
            std::cout << "Registering commands...\n";
            co_await util::create_commands(bot);
        }

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
