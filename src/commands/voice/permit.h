#pragma once
#include <dpp/dpp.h>

namespace commands::permit {
    dpp::task<void> execute(const dpp::slashcommand_t& event);
}
