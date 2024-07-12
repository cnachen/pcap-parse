#include <string>
#include <cmath>
#include <sys/stat.h>

#include <archive.h>
#include <archive_entry.h>

#include "workers/obs_observer.h"

using namespace std::literals::chrono_literals;

void ObsObserver::work() {
    while (running) {
        if (loop_count >= loop_limit) {
            loop_count = 0;
            printf("Refreshing hostnames\n");
            // hostnames.clear();
            // auto x = get_hostnames("");
            // hostnames.swap(x);
            for (int i = 0; i < hostnames.size(); i++) {
                printf("%s ", hostnames[i].c_str());
            }
            printf("\n");

            for (auto hostname : hostnames) {
                mkdir(std::string("/tmp/" + hostname).c_str(), 0755);
            }

            printf("Refreshing files\n");
            while (!remote_files.empty())
                remote_files.pop();

            for (auto hostname : hostnames) {
                for (auto file : list_files_by_hostname(hostname)) {
                    printf("Fetching url: %s/%s\n", hostname.c_str(), file.c_str());
                    auto url = get_file_url(file);
                    remote_files.push(RemoteFile {
                        .hostname = hostname,
                        .url = url,
                        .uuid = generate_uuid(),
                    });
                    printf("Url fetched: %s\n", url.c_str());
                }
            }
        }

        if (!remote_files.empty()) {
            auto remote_file = remote_files.front();
            remote_files.pop();
            printf("Downloading: %s\n", remote_file.url.c_str());
            std::string local_path = "/tmp/" + remote_file.hostname + "/" + remote_file.uuid; 
            download_file(remote_file.url, local_path);
            printf("Downloaded: %s\n", local_path.c_str());
            printf("Extracting: %s\n", local_path.c_str());
            auto pcap = extract_file(local_path, "/tmp/" + remote_file.hostname);
            printf("Extracted: %s\n", pcap.c_str());
            locked_queue->push(LocalFile {
                .hostname = remote_file.hostname,
                .path = pcap,
            });
        }

        std::this_thread::sleep_for(1s);
        loop_count++;
    }
}

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

std::string ObsObserver::extract_file(const std::string &path, const std::string &local) {
    struct archive *a;
    struct archive_entry *entry;
    std::string full_path;
    int r;

    a = archive_read_new();
    archive_read_support_format_tar(a);
    archive_read_support_filter_gzip(a);
    archive_read_support_filter_xz(a);

    r = archive_read_open_filename(a, path.c_str(), 10240);
    if (r != ARCHIVE_OK) {
        fprintf(stderr, "Error opening the file: %s\n", archive_error_string(a));
    }

    if (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        full_path = local + "/" + archive_entry_pathname(entry);
        archive_entry_set_pathname(entry, full_path.c_str());
        r = archive_read_extract(a, entry, ARCHIVE_EXTRACT_TIME);
        if (r != ARCHIVE_OK) {
            fprintf(stderr, "Error extracting the file: %s\n", archive_error_string(a));
        }
    }

    archive_read_close(a);
    archive_read_free(a);

    return full_path;
}
