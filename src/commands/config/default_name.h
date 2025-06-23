#pragma once
#include <dpp/dpp.h>

namespace commands::config::default_name {
    dpp::task<void> set(const dpp::slashcommand_t& event);
    dpp::task<void> reset(const dpp::slashcommand_t& event);
}
