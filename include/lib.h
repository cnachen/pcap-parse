#ifndef _LIB_H
#define _LIB_H

#include <jsoncpp/json/json.h>
#include <cpr/cpr.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <random>
#include <unistd.h>

#include "utility.h"

using byte = unsigned char;

class Api {
private:
    const std::string api_server;
public:
    Api(const std::string api_server = "https://dev.common.bip.momenta.works") : api_server(api_server) {}
    std::string get_obs_file_url(const std::string &path);
    std::string list_obs_files(const std::string &hostname, int page, int pagesize = 10);
    std::string list_obs_hostnames(int page, int pagesize = 10);
    void post_mongodb(Json::Value &json);
};

struct Global {
    std::array<char, 64> hostname;
    Json::Value root;
    LockedQueue<Json::Value> jsons;

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

static inline std::string generate_uuid() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_int_distribution<> dis2(8, 11);

    std::stringstream ss;
    ss << std::hex;

    for (int i = 0; i < 8; i++) {
        ss << dis(gen);
    }
    ss << "-";

    for (int i = 0; i < 4; i++) {
        ss << dis(gen);
    }
    ss << "-4";

    for (int i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";

    ss << dis2(gen);

    for (int i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";

    for (int i = 0; i < 12; i++) {
        ss << dis(gen);
    }

    return ss.str();
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

#endif /* _LIB_H */
