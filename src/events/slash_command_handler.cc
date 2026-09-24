#include "events/slash_command_handler.h"
#include <dpp/dpp.h>
#include <unordered_map>
#include <functional>

#include "commands/voice/lock.h"
#include "commands/voice/unlock.h"
#include "commands/voice/name.h"
#include "commands/voice/limit.h"
#include "commands/voice/permit.h"
#include "commands/voice/claim.h"
#include "commands/voice/deny.h"

#include "commands/config/default_name.h"

#include "db/voice_interface.h"
#include "types.h"

std::unordered_map<std::string, std::function<dpp::task<void>(const dpp::slashcommand_t&)>> voice_commands = {
    {"lock", commands::lock::execute},
    {"unlock", commands::unlock::execute},
    {"name", commands::name::execute},
    {"limit", commands::limit::execute},
    {"permit", commands::permit::execute},
    {"claim", commands::claim::execute},
    {"deny", commands::deny::execute}
};

std::unordered_map<config_option, std::function<dpp::task<void>(const dpp::slashcommand_t&)>> config_set_commands = {
    {config_option::default_apt_name, commands::config::default_name::set}
};

std::unordered_map<config_option, std::function<dpp::task<void>(const dpp::slashcommand_t&)>> config_reset_commands = {
    {config_option::default_apt_name, commands::config::default_name::reset}
};

namespace events {
    dpp::task<void> handle_slash_command(const dpp::slashcommand_t &event) {
        if (event.command.get_command_name() == "voice") {
            auto subcommand = event.command.get_command_interaction().options[0];

            if (voice_commands.find(subcommand.name) != voice_commands.end()) {
                co_await voice_commands[subcommand.name](event);
            }
        }
        else if (event.command.get_command_name() == "config") {
            auto subcommand = event.command.get_command_interaction().options[0];

            if (subcommand.name == "set") {
                auto option = subcommand.get_value<int64_t>(0);
                auto option_val = static_cast<config_option>(option);

                if (config_set_commands.find(option_val) != config_set_commands.end()) {
                    co_await config_set_commands[option_val](event);
                }
            }
            else if (subcommand.name == "reset") {
                auto option = subcommand.get_value<int64_t>(0);
                auto option_val = static_cast<config_option>(option);

                if (config_reset_commands.find(option_val) != config_reset_commands.end()) {
                    co_await config_reset_commands[option_val](event);
                }
            }
        }
    }
}
