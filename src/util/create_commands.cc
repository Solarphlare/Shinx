#include "util/create_commands.h"
#include <dpp/dpp.h>
#include <cstdint>
#include <iostream>
#include <vector>
#include "types.h"

namespace util {
    dpp::task<void> create_commands(dpp::cluster& bot) {
        dpp::slashcommand voice("voice", "Manage your apartment", bot.me.id);
        voice.set_interaction_contexts({dpp::itc_guild});
        voice.add_option(dpp::command_option(dpp::co_sub_command, "lock", "Lock your apartment"));
        voice.add_option(dpp::command_option(dpp::co_sub_command, "unlock", "Unlock your apartment"));
        voice.add_option(
            dpp::command_option(dpp::co_sub_command, "name", "Rename your apartment")
                .add_option(
                    dpp::command_option(dpp::co_string, "name", "The new apartment name", true)
                        .set_min_length(int64_t{1})
                        .set_max_length(int64_t{100})
                )
        );
        voice.add_option(
            dpp::command_option(dpp::co_sub_command, "limit", "Set your apartment's user limit")
                .add_option(
                    dpp::command_option(dpp::co_integer, "limit", "The user limit, or 0 to remove the limit", true)
                        .set_min_value(int64_t{0})
                        .set_max_value(int64_t{99})
                )
        );
        voice.add_option(
            dpp::command_option(dpp::co_sub_command, "permit", "Allow a user to join your apartment")
                .add_option(dpp::command_option(dpp::co_user, "user", "The user to allow", true))
        );
        voice.add_option(dpp::command_option(dpp::co_sub_command, "claim", "Claim an apartment after its owner leaves"));
        voice.add_option(
            dpp::command_option(dpp::co_sub_command, "deny", "Deny a user access to your apartment")
                .add_option(dpp::command_option(dpp::co_user, "user", "The user to deny", true))
        );

        dpp::command_option config_setting(dpp::co_integer, "option", "The setting to change", true);
        config_setting.add_choice(
            dpp::command_option_choice("Default Apartment Name", static_cast<int64_t>(config_option::default_apt_name))
        );

        dpp::slashcommand config("config", "Manage your apartment preferences", bot.me.id);
        // The handlers read the setting at index 0 and its new value at index 1.
        config.add_option(
            dpp::command_option(dpp::co_sub_command, "set", "Set an apartment preference")
                .add_option(config_setting)
                .add_option(
                    dpp::command_option(dpp::co_string, "value", "The new default apartment name", true)
                        .set_min_length(int64_t{1})
                        .set_max_length(int64_t{32})
                )
        );
        config.add_option(
            dpp::command_option(dpp::co_sub_command, "reset", "Reset an apartment preference")
                .add_option(config_setting)
        );

        // This replaces the application's complete global command list.
        const std::vector<dpp::slashcommand> commands{voice, config};
        dpp::confirmation_callback_t callback = co_await bot.co_global_bulk_command_create(commands);
        if (callback.is_error()) {
            std::cerr << "Failed to register slash commands: " << callback.get_error().human_readable << "\n";
            co_return;
        }

        std::cout << "Slash commands registered successfully.\n";
    }
}
