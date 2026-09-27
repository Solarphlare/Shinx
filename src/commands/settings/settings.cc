#include "commands/settings/settings.h"
#include <dpp/dpp.h>
#include <bsoncxx/builder/basic/document.hpp>
#include <random>
#include <dpp/json.h>
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
        auto settings_doc = db["misc"].find_one(bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("type", "settings")
        ));

        // DPP doesn't have support for label descriptions yet,
        // so we send raw JSON instead of using DPP methods here.
        dpp::json payload = {
            {"type", dpp::ir_modal_dialog},
            {"data", {
                {"title", "Settings"},
                {"custom_id", "settings_modal_" + generate_random_hex_string()},
                {"components", {
                    {
                        {"type", dpp::cot_label},
                        {"label", "Lobby Channel"},
                        {"description", "The voice channel users can join to create a new apartment channel."},
                        {"component", {
                            {"type", dpp::cot_channel_selectmenu},
                            {"custom_id", "lobby_channel_id"},
                            {"channel_types", {dpp::channel_type::CHANNEL_VOICE}},
                            {"max_values", 1U},
                            {"required", true}
                        }}
                    },
                    {
                        {"type", dpp::cot_label},
                        {"label", "Apartment Category"},
                        {"description", "THe category in which apartment channels should be created."},
                        {"component", {
                            {"type", dpp::cot_channel_selectmenu},
                            {"custom_id", "apartment_category_id"},
                            {"channel_types", {dpp::channel_type::CHANNEL_CATEGORY}},
                            {"max_values", 1U},
                            {"required", true}
                        }}
                    }
                }}
            }},
        };

        // set default values
        if (settings_doc) {
            auto lobby_channel_id = settings_doc->view()["lobby_channel_id"];
            auto apartment_category_id = settings_doc->view()["apartment_category_id"];

            if (lobby_channel_id) {
                payload["data"]["components"][0]["component"]["default_values"] = {
                    {
                        {"id", static_cast<std::string>(lobby_channel_id.get_string().value)},
                        {"type", "channel"}
                    }
                };
            }

            if (apartment_category_id) {
                payload["data"]["components"][1]["component"]["default_values"] = {
                    {
                        {"id", static_cast<std::string>(apartment_category_id.get_string().value)},
                        {"type", "channel"}
                    }
                };
            }
        }

        event.owner->post_rest(
            API_PATH "/interactions",
            std::to_string(event.command.id),
            dpp::utility::url_encode(event.command.token) + "/callback",
            dpp::m_post,
            payload.dump(),
            [](dpp::json& response, const dpp::http_request_completion_t& http) {
                if (http.status < 200 || http.status > 299) {
                    std::cerr << "Failed to show modal: (" << http.status << ") " << http.body << '\n';
                }
            }
        );
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
