#include <stdlib.h>

struct file_watcher_ctx;
//      void   callback (const char* fname); // normal func dec
typedef int (*file_changed_callback)(const char*      ); // func. pointer type named "callback"

struct file_watcher_ctx* fw_init();
int               fw_watch(struct file_watcher_ctx* ctx, const char* fname, file_changed_callback cb);
void              fw_stop_watch(struct file_watcher_ctx* ctx, const char* fname);
void              fw_shutdown(struct file_watcher_ctx* ctx);

