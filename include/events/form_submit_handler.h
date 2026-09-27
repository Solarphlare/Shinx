#pragma once
#include <dpp/dpp.h>

namespace events {
    dpp::task<void> handle_form_submit(const dpp::form_submit_t& event);
}
