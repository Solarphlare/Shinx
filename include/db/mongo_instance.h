#pragma once
#include <mongocxx/instance.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/uri.hpp>

namespace db {
    mongocxx::database& get_database();
}
