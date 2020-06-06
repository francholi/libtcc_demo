#include "file_utils.h"
#include "file_watcher.h"

//struct file_watcher_ctx;
//typedef void (*callback)(const char*      ); // func. pointer type named "callback"

struct file_watcher_ctx* fw_init()
{
    return NULL;
}

int fw_watch(struct file_watcher_ctx* ctx, const char* fname, callback cb, unsigned int freq_ms)
{
    return 0;
}

void fw_stop_watch(struct file_watcher_ctx* ctx, const char* fname)
{
    return;
}

void fw_shutdown(struct file_watcher_ctx* ctx)
{
    return;
}

