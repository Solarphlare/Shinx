#pragma once
#include <vector>
#include <dpp/snowflake.h>

namespace db::helpers {
    void remove_stale_apartments(const std::vector<dpp::snowflake>& stale_apartments);
}
