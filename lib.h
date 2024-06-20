#ifndef _LIB_H
#define _LIB_H

#include <jsoncpp/json/json.h>
#include <cpr/cpr.h>
#include <string>
#include <algorithm>
#include <iostream>

using byte = unsigned char;

static const std::string api_server = "https://yapi.mmtwork.com/mock/607/common/insert-documents/pcap_data";
static const int max_records = 32;

struct Global {
    std::array<char, 64> hostname;
    Json::Value root;

    Global() {
        // Get hostname for one time
        gethostname(hostname.data(), hostname.size());
    }
};

static inline std::string tohex(const byte *input, uint32_t len) {
    std::string output;
    static const char digits[] = "0123456789abcdef";

    output.reserve(len * 2);

    for (int i = 0; i < len; i++) {
        byte c = input[i];

        output.push_back(digits[c >> 4]);
        output.push_back(digits[c & 15]);
    }

    return output;
}

static inline std::string format_json_string(const Json::Value &json) {
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "   ";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::ostringstream os;
    writer->write(json, &os);
    return os.str();
}

static inline void post_mongodb(Global &g, Json::Value &json) {
    if (!json.isArray() || json.size() < max_records)
        return;

    cpr::Header headers {{"Content-Type", "application/json"}};

    auto post = [&]() {
        return cpr::Post(
            cpr::Url {api_server},
            cpr::Body {format_json_string(json).c_str()},
            headers
        );
    };

    int retries = 3;
    while (retries--) {
        auto r = post();

        if (r.status_code >= 200 && r.status_code < 300) {
            printf("Success: %s\n", r.text.c_str());
            break;
        }

        printf("Failed(%ld): %s\n", r.status_code, r.error.message.c_str());
    }

    json.clear();
}

#endif /* _LIB_H */