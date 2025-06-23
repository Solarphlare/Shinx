#pragma once
#include <dpp/dpp.h>

namespace commands::deny {
    dpp::task<void> execute(const dpp::slashcommand_t& event);
}

