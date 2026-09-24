#include <dpp/dpp.h>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <functional>
#include "db/mongo_instance.h"
#include "util/util.h"
#include "db/helpers.h"

namespace db::voice {
    dpp::task<std::unordered_map<dpp::snowflake, dpp::snowflake>> get_apartments(dpp::cluster* bot) {
        mongocxx::database& database = db::get_database();
        auto cursor = database["apartments"].find({});

        std::unordered_map<dpp::snowflake, dpp::snowflake> apartments;
        std::vector<dpp::snowflake> stale_apartments;

        for (const auto& doc : cursor) {
            dpp::snowflake apartment_id = static_cast<std::string>(doc["_id"].get_string().value);
            dpp::snowflake owner_id = doc["owner_id"].get_int64().value;
            dpp::snowflake guild_id = doc["guild_id"].get_int64().value;

            dpp::guild guild;

            try {
                guild = co_await util::get_guild(bot, guild_id);
            }
            catch (const std::exception& e) {
                std::cerr << e.what() << "\n";
                continue;
            }

            if (std::find(guild.channels.begin(), guild.channels.end(), apartment_id) == guild.channels.end()) {
                // Channel not found in the guild, mark apartment as stale
                stale_apartments.push_back(apartment_id);
                continue;
            }

            apartments[apartment_id] = owner_id;
        }

        if (stale_apartments.size() > 0) {
            std::cout << "Found " << stale_apartments.size() << " stale apartment(s), removing them from the database.\n";
            bsoncxx::builder::basic::array document_ids{};
            for (const auto& apartment_id : stale_apartments) {
                document_ids.append(apartment_id.str());
            }

            auto filter_builder = bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("_id", bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("$in", document_ids)
                ))
            );

            mongocxx::database& database = db::get_database();
            database["apartments"].delete_many(filter_builder.view());

            std::cout << "Removed " << stale_apartments.size() << " stale apartment(s) from the database.\n";
        }

        if (!stale_apartments.empty()) {
            std::thread(db::helpers::remove_stale_apartments, std::ref(stale_apartments)).detach();
        }

        co_return apartments;
    }

    void add_apartment(const dpp::snowflake& owner_id, const dpp::snowflake& apartment_id, const dpp::snowflake& guild_id) {
        mongocxx::database& database = db::get_database();
        bsoncxx::builder::basic::document doc_builder;
        doc_builder.append(
            bsoncxx::builder::basic::kvp("_id", apartment_id.str()),
            bsoncxx::builder::basic::kvp("owner_id", (int64_t) owner_id),
            bsoncxx::builder::basic::kvp("guild_id", (int64_t) guild_id)
        );

        database["apartments"].insert_one(doc_builder.view());
    }

    void remove_apartment(const dpp::snowflake& apartment_id) {
        mongocxx::database& database = db::get_database();
        bsoncxx::builder::basic::document filter_builder;
        filter_builder.append(bsoncxx::builder::basic::kvp("_id", apartment_id.str()));

        database["apartments"].delete_one(filter_builder.view());
    }

    void transfer_apartment(const dpp::snowflake& apartment_id, const dpp::snowflake& new_owner_id) {
        mongocxx::database& database = db::get_database();
        bsoncxx::builder::basic::document filter_builder;
        filter_builder.append(bsoncxx::builder::basic::kvp("_id", apartment_id.str()));

        bsoncxx::builder::basic::document update_builder;
        update_builder.append(
            bsoncxx::builder::basic::kvp("$set", bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("owner_id", (int64_t) new_owner_id)
            ))
        );

        database["apartments"].update_one(filter_builder.view(), update_builder.view());
    }
}
