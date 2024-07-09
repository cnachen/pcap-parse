#include <string>
#include <cmath>

#include "workers/obs_observer.h"

void ObsObserver::download_file(const std::string &url, const std::string &local_path) {
    cpr::Response r = cpr::Get(cpr::Url{url});
    FILE *f = fopen(local_path.c_str(), "wb");
    fwrite(r.text.c_str(), 1, r.text.size(), f);
    fclose(f);
}

std::string ObsObserver::get_file_url(const std::string &path) {
    std::string ret = api.get_obs_file_url(path);
    Json::Value tmp;
    Json::Reader reader;

    reader.parse(ret, tmp);

    return tmp["data"]["url"].asString();
}

std::vector<std::string> ObsObserver::list_files_by_hostname(const std::string &hostname) {
    std::vector<std::string> files;

    std::string ret = api.list_obs_files(hostname, 0);
    Json::Value tmp;
    Json::Reader reader;

    reader.parse(ret, tmp);

    int total = tmp["data"]["total"].asInt();

    for (int page = 0; page <= ceil((double)total / pagesize); page++) {
        tmp.clear();
        reader.parse(api.list_obs_files(hostname, page, pagesize), tmp);
        for (auto record : tmp["data"]["records"]) {
            files.push_back(record.asString());
        }
    }

    return files;
}

std::vector<std::string> ObsObserver::get_hostnames(const std::string &path) {
    std::vector<std::string> hostnames;

    std::string ret = api.list_obs_hostnames(0);
    Json::Value tmp;
    Json::Reader reader;

    reader.parse(ret, tmp);

    int total = tmp["data"]["total"].asInt();

    for (int page = 0; page <= ceil((double)total / pagesize); page++) {
        tmp.clear();
        reader.parse(api.list_obs_hostnames(page, pagesize), tmp);
        for (auto record : tmp["data"]["records"]) {
            hostnames.push_back(record.asString());
        }
    }

    return hostnames;
}
