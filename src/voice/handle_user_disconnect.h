#pragma once
#include <dpp/dpp.h>

namespace voice {
    dpp::task<void> handle_user_disconnect(const dpp::voice_state_update_t& event);
}
