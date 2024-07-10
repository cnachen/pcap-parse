#include "lib.h"

extern Global g;

std::string Api::get_obs_file_url(const std::string &path) {
    cpr::Header headers {{"Content-Type", "application/json"}};

    auto get = [&]() {
        return cpr::Get(
            cpr::Url {api_server + "/common/get-file-url?object_key=" + path},
            headers
        );
    };

    return get().text;
}

std::string Api::list_obs_files(const std::string &hostname, int page, int pagesize) {
    cpr::Header headers {{"Content-Type", "application/json"}};

    auto get = [&]() {
        return cpr::Get(
            cpr::Url {api_server + "/common/file-lists?tool_name=eck&hostname=" + hostname + "&page_size=" + std::to_string(pagesize) + "&page=" + std::to_string(page)},
            headers
        );
    };

    return get().text;
}

std::string Api::list_obs_hostnames(int page, int pagesize) {
    cpr::Header headers {{"Content-Type", "application/json"}};

    auto get = [&]() {
        return cpr::Get(
            cpr::Url {api_server + "/common/file-lists?query=eck/eth&page_size=" + std::to_string(pagesize) + "&page=" + std::to_string(page)},
            headers
        );
    };

    return get().text;
}

void Api::post_mongodb(Json::Value &json) {
    cpr::Header headers {{"Content-Type", "application/json"}};

    auto post = [&]() {
        return cpr::Post(
            cpr::Url {api_server + "/common/insert-documents/eck_mirror_data_test"},
            cpr::Body {format_json_string(json).c_str()},
            headers
        );
    };

    int retries = 3;
    bool success = false;
    while (retries--) {
        auto r = post();

        if (r.status_code >= 200 && r.status_code < 300) {
            printf("Success: %s\n", r.text.c_str());
            success = true;
            break;
        }
        printf("Failed(%ld): %s\n", r.status_code, r.error.message.c_str());
    }

    if (!success) {
        // Handle network error
    }

    json.clear();
}
