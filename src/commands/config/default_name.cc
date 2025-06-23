#include "default_name.h"
#include <dpp/dpp.h>
#include "db/mongo_instance.h"
#include <bsoncxx/builder/stream/document.hpp>

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

namespace commands::config::default_name {
    dpp::task<void> set(const dpp::slashcommand_t& event) {
        auto subcommand = event.command.get_command_interaction().options[0];
        std::string new_default_name = subcommand.get_value<std::string>(1);

        if (new_default_name.length() > 32) {
            co_await event.co_reply(dpp::message("The name can't be longer than 32 characters.").set_flags(dpp::m_ephemeral));
            co_return;
        }

        auto& database = db::get_database();
        database["user_config"].update_one(
            make_document(kvp("_id", event.command.usr.id.str())),
            make_document(kvp("$set", make_document(kvp("default_apartment_name", new_default_name)))),
            mongocxx::options::update().upsert(true)
        );

        co_await event.co_reply(dpp::message("Your default apartment name has been set to \"" + new_default_name + "\".").set_flags(dpp::m_ephemeral));
    }

    dpp::task<void> reset(const dpp::slashcommand_t& event) {
        auto& database = db::get_database();

        auto result = database["user_config"].find_one(make_document(kvp("_id", event.command.usr.id.str())));
        if (!result || result->find("default_apartment_name") == result->end()) {
            co_await event.co_reply(dpp::message("You don't have a default apartment name set.").set_flags(dpp::m_ephemeral));
            co_return;
        }

        database["user_config"].update_one(
            make_document(kvp("_id", event.command.usr.id.str())),
            make_document(kvp("$unset", make_document(kvp("default_apartment_name", ""))))
        );

        co_await event.co_reply(dpp::message("Your default apartment name has been reset.").set_flags(dpp::m_ephemeral));
    }
}
