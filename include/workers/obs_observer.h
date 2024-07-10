#ifndef _WORKERS_OBS_OBSERVER_H
#define _WORKERS_OBS_OBSERVER_H

#include <vector>
#include <string>

#include "workers/worker.h"
#include "lib.h"

struct RemoteFile {
    std::string hostname;
    std::string url;
    std::string uuid;
};

class ObsObserver : public Worker {
private:
    LockedQueue<std::string> *locked_queue;
    Api api;
    std::vector<std::string> hostnames;
    std::queue<RemoteFile> remote_files;
    int loop_count = 99;
    int loop_limit = 100;
    int pagesize = 50;
protected:
    void work() override;
public:
    ObsObserver(LockedQueue<std::string> *locked_queue)
        : locked_queue(locked_queue) {
            // hostnames.push_back("LC6-EDY0038");
            hostnames.push_back("Test");
        }
    std::vector<std::string> list_files_by_hostname(const std::string &hostname);
    std::vector<std::string> get_hostnames(const std::string &path);
    void download_file(const std::string &path, const std::string &local_path);
    std::string get_file_url(const std::string &path);
    std::string extract_file(const std::string &path, const std::string &local);
};

#endif
