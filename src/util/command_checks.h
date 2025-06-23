#pragma once
#include <dpp/dpp.h>

namespace util {
    dpp::task<bool> check_voice_command_requirements(const dpp::slashcommand_t& event);
}
