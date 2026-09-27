#include "db/mongo_instance.h"
#include <dpp/guild.h>
#include <cstdlib>
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/database.hpp>

const mongocxx::instance driver_instance{};
const mongocxx::client client{mongocxx::uri{std::getenv("MONGO_URI")}};

namespace db {
    mongocxx::database get_database(const dpp::snowflake& guild_id) {
        #ifdef DEBUG
        return client["shinx_" + guild_id.str() + "_test"];
        #else
        return client["shinx_" + guild_id.str()];
        #endif
    }
}
