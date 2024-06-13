#ifndef _LIB_H
#define _LIB_H

#include <jsoncpp/json/json.h>
#include <string>

using byte = unsigned char;

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

#endif /* _LIB_H */