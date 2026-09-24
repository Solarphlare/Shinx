#pragma once
#include <dpp/dpp.h>

namespace util {
    dpp::task<void> build_voice_state_cache(dpp::cluster* bot);
}
