#pragma once
#include <dpp/dpp.h>

namespace commands::unlock {
    dpp::task<void> execute(const dpp::slashcommand_t& event);
}
