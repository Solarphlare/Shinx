#include "db/helpers.h"
#include <vector>
#include <dpp/snowflake.h>
#include <mongocxx/client.hpp>
#include <thread>
#include <bsoncxx/builder/basic/document.hpp>

namespace db::helpers {
    void remove_stale_apartments(const std::vector<dpp::snowflake>& stale_apartments) {
    if (stale_apartments.empty()) return;

    bsoncxx::builder::basic::array document_ids{};
    for (const auto& apartment_id : stale_apartments) {
        document_ids.append(apartment_id.str());
    }

    auto filter_builder = bsoncxx::builder::basic::make_document(
        bsoncxx::builder::basic::kvp("_id", bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("$in", document_ids)
        ))
    );

    const mongocxx::client client{mongocxx::uri{std::getenv("MONGO_URI")}};

    #ifdef DEBUG
    mongocxx::database database = client["riolu_test"];
    #else
    mongocxx::database database = client["riolu"];
    #endif

    database["apartments"].delete_many(filter_builder.view());
}

}
