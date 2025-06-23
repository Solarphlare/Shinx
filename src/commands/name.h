#pragma once
#include <dpp/dpp.h>

namespace commands::name {
    dpp::task<void> execute(const dpp::slashcommand_t& event);
}
