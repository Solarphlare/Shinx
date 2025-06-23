#pragma once
#include <dpp/dpp.h>

namespace events {
    dpp::task<void> handle_slash_command(const dpp::slashcommand_t &event);
}
