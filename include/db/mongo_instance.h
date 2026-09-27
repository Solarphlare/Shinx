#pragma once
#include <dpp/guild.h>
#include <mongocxx/instance.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/uri.hpp>

namespace db {
    mongocxx::database get_database(const dpp::snowflake& guild_id);
}
