#pragma once
#include <dpp/dpp.h>

namespace events {
    dpp::task<void> handle_voice_state_update(const dpp::voice_state_update_t &event);
}
