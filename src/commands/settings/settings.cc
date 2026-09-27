#include "commands/settings/settings.h"
#include <dpp/dpp.h>
#include <bsoncxx/builder/basic/document.hpp>
#include <random>
#include "db/mongo_instance.h"

static const char hex_characters[] = "0123456789abcdef";
std::mt19937 generator((std::random_device())());
std::uniform_int_distribution<std::size_t> distribution(0, 15);
std::string generate_random_hex_string();

namespace commands::settings {
    dpp::task<void> execute(const dpp::slashcommand_t& event) {
        if (!event.command.member.is_guild_owner() && (event.command.get_guild().base_permissions(event.command.member) & 8) != 8) {
            co_await event.co_reply(dpp::message("You don't have permission to use this command!").set_flags(dpp::m_ephemeral));
        }

        mongocxx::database db = db::get_database(event.command.guild_id);
        auto settings_doc = db.collection("misc").find_one(bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("type", "settings")
        ));

        dpp::interaction_modal_response modal("settings_modal_" + generate_random_hex_string(), "Settings");

        auto lobby_channel_select = dpp::component()
            .set_label("Lobby Channel")
            .set_id("lobby_channel_id")
            .set_type(dpp::cot_channel_selectmenu)
            .set_description("The voice channel users can join to create a new apartment.")
            .add_channel_type(dpp::channel_type::CHANNEL_VOICE)
            .set_required(true)
            .set_max_values(1);

        if (settings_doc) {
            auto lobby_channel_id = settings_doc->view()["lobby_channel_id"];

            if (lobby_channel_id) {
                lobby_channel_select.add_default_value(static_cast<std::string>(lobby_channel_id.get_string().value), dpp::component_default_value_type::cdt_channel);
            }
        }

        modal.add_component(lobby_channel_select);

        auto apartment_category_id_select = dpp::component()
            .set_label("Apartment Category")
            .set_id("apartment_category_id")
            .set_description("The category in which apartments should be created.")
            .set_type(dpp::cot_channel_selectmenu)
            .add_channel_type(dpp::channel_type::CHANNEL_CATEGORY)
            .set_required(true)
            .set_max_values(1);


        if (settings_doc) {
            auto apartment_category_id = settings_doc->view()["apartment_category_id"];

            if (apartment_category_id) {
                apartment_category_id_select.add_default_value(static_cast<std::string>(apartment_category_id.get_string().value), dpp::component_default_value_type::cdt_channel);
            }
        }

        modal.add_component(apartment_category_id_select);

        co_await event.co_dialog(modal);
    }
}

std::string generate_random_hex_string() {
    std::string result;
    result.reserve(8);

    for (int i = 0; i < 8; i++) {
        result += hex_characters[distribution(generator)];
    }

    return result;
}
