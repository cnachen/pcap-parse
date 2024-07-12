#ifndef _WORKERS_MONGODB_UPLOADER_H
#define _WORKERS_MONGODB_UPLOADER_H

#include "workers/worker.h"
#include "lib.h"

class MongodbUploader : public Worker {
private:
    Api api;
    int multiplier = 1;
    int chunk_size = 8192;
protected:
    void work() override;
public:
    void upload(Json::Value &json);
};

#endif
