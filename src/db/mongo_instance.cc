#include "db/mongo_instance.h"
#include <cstdlib>
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/database.hpp>

const mongocxx::instance driver_instance{};
const mongocxx::client client{mongocxx::uri{std::getenv("MONGO_URI")}};

#ifdef DEBUG
mongocxx::database database = client["riolu_test"];
#else
mongocxx::database database = client["riolu"];
#endif

namespace db {
    mongocxx::database& get_database() {
        return database;
    }
}
