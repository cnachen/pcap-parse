#include "workers/mongodb_uploader.h"

extern Global g;

using namespace std::literals::chrono_literals;

void MongodbUploader::work() {
    while (running) {
        auto [not_default, json] = g.jsons.pop_default();
        if (not_default) {
            upload(json);
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