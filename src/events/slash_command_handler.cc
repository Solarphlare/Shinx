#include <dpp/dpp.h>
#include <unordered_map>
#include <functional>

#include "commands/lock.h"
#include "commands/unlock.h"
#include "commands/name.h"
#include "commands/limit.h"
#include "commands/permit.h"
#include "commands/claim.h"

#include "db/voice_interface.h"

std::unordered_map<std::string, std::function<dpp::task<void>(const dpp::slashcommand_t&)>> voice_commands = {
    {"lock", commands::lock::execute},
    {"unlock", commands::unlock::execute},
    {"name", commands::name::execute},
    {"limit", commands::limit::execute},
    {"permit", commands::permit::execute},
    {"claim", commands::claim::execute}
};

namespace events {
    dpp::task<void> handle_slash_command(const dpp::slashcommand_t &event) {
        if (event.command.get_command_name() == "voice") {
            auto subcommand = event.command.get_command_interaction().options[0];

            if (voice_commands.find(subcommand.name) != voice_commands.end()) {
                co_await voice_commands[subcommand.name](event);
            }
        }
    }
}
