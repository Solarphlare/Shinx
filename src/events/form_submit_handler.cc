#include "events/form_submit_handler.h"

#include <dpp/dpp.h>
#include <algorithm>
#include <bsoncxx/builder/basic/document.hpp>

#include "db/mongo_instance.h"

namespace events {
    dpp::task<void> handle_form_submit(const dpp::form_submit_t& event) {
        auto lobby_channel_id = std::find_if(event.components.begin(), event.components.end(), [](const dpp::component& component) {
            return component.custom_id == "lobby_channel_id";
        });

        auto apartment_category_id = std::find_if(event.components.begin(), event.components.end(), [](const dpp::component& component) {
            return component.custom_id == "apartment_category_id";
        });

        if (lobby_channel_id == event.components.end() || apartment_category_id == event.components.end()) co_return;

        mongocxx::options::update options;
        options.upsert(true);

        db::get_database(event.command.guild_id)["misc"].update_one(
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("type", "settings")
            ),
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("$set",
                    bsoncxx::builder::basic::make_document(
                        bsoncxx::builder::basic::kvp("lobby_channel_id", std::get<std::string>(lobby_channel_id->value)),
                        bsoncxx::builder::basic::kvp("apartment_category_id", std::get<std::string>(apartment_category_id->value))
                    )
                )
            ),
            options
        );

        co_await event.co_reply(dpp::message("Your settings have been saved.").set_flags(dpp::m_ephemeral));
    }
}
