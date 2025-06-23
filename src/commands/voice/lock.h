#pragma once
#include <dpp/dpp.h>

namespace commands::lock {
    dpp::task<void> execute(const dpp::slashcommand_t& event);
}
