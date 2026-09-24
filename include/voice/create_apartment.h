#pragma once
#include <dpp/dpp.h>

namespace voice {
    dpp::task<void> create_apartment(const dpp::voice_state_update_t& event);
}
