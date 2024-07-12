#include "workers/mongodb_uploader.h"

extern Global g;

using namespace std::literals::chrono_literals;

void MongodbUploader::work() {
    while (running) {
        Json::Value send_list;
        bool have_data = false;

        for (int i = 0; i < chunk_size; i++) {
            auto [not_default, json] = g.jsons.pop_default();
            if (not_default) {
                send_list.append(json);
                have_data = true;
            }
        }

        if (have_data) {
            upload(send_list);
            multiplier = 1;
        } else if (multiplier < 16) {
            multiplier *= 2;
        }

        std::this_thread::sleep_for(multiplier * 50ms);
    }
}

void MongodbUploader::upload(Json::Value &json) {
    if (!json.isArray())
        return;
    api.post_mongodb(json);
}