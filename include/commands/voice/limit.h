#pragma once
#include <dpp/dpp.h>

namespace commands::limit {
    dpp::task<void> execute(const dpp::slashcommand_t& event);
}
