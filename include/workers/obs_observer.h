#ifndef _WORKERS_OBS_OBSERVER_H
#define _WORKERS_OBS_OBSERVER_H

#include <vector>

#include "workers/worker.h"
#include "lib.h"

class ObsObserver : public Worker {
private:
    Api api;
    int pagesize = 50;
public:
    std::vector<std::string> list_files_by_hostname(const std::string &hostname);
    std::vector<std::string> get_hostnames(const std::string &path);
    void download_file(const std::string &path, const std::string &local_path);
    std::string get_file_url(const std::string &path);
};

#endif
