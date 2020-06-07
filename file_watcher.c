#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "file_utils.h"
#include "file_watcher.h"

#define MAX_FILE_ENTRIES 255
//typedef void (*callback)(const char*      ); // func. pointer type named "callback"

// keep track of files, timestamps, and a callback per file.
typedef struct file_watcher_entry {
    size_t         is_valid; // 0 is invalid, 1 is valid.
    char         filename[255];
    struct stat  filestats;
    callback cb;
} file_watcher_entry;

typedef struct file_watcher_ctx
{
  file_watcher_entry file_entries[MAX_FILE_ENTRIES];
  unsigned int entry_count;
} file_watcher_ctx;

static file_watcher_ctx g_context;
pthread_t watcher_thread;

size_t g_stop_thread_flag = 0;
static void* watch_files_thread(void* data)
{
    while(g_stop_thread_flag == 0)
    {
        usleep(1000000); // 1.0s

        for (int f = 0; f < g_context.entry_count; f++)
        {
            struct file_watcher_entry *entry = &g_context.file_entries[f];
            if (file_changed( entry->filename, &entry->filestats) && (entry->is_valid==1))
            {
                entry->cb((void*)entry->filename);
            }
        }
    }
}

struct file_watcher_ctx* fw_init()
{
    // do not allow more than one context for now
    if (g_context.entry_count != 0)
    {
        return &g_context;
    }
    g_context.entry_count = 0;
    pthread_create(&watcher_thread, NULL, watch_files_thread, NULL);
    return &g_context;
}

// return 0 on success, -1 if any error/warning
int fw_watch(struct file_watcher_ctx* ctx, const char* fname, callback cb)
{
    // check if there are entries available:
    // TODO: Either check for holes (invalid entries) in the range, or reimplement
    if (ctx->entry_count == MAX_FILE_ENTRIES)
    {
        fprintf(stderr, "FATAL ERROR, NO MORE FILE ENTRIES AVAILABLE, FIX YOUR CODE!\n");
        return -1;
    }

    // check all the names in the ctx, if it is registered already, ignore.;
    // if is a new filename, register a new entry, which the thread has to loop through.
    // (beware contention here!, fw_watch is called from the main thread!)

    // check if the file is already registered.
    for (int i = 0; i < ctx->entry_count; i++)
    {
        // for now, to deal with holes just ignore "invalid" entries
        if (ctx->file_entries[i].is_valid == 0)
                continue;

        // if entry exists, keep it as it is, but warn about mistake
        if (strcmp(ctx->file_entries[i].filename, fname) == 0)
        {
            fprintf(stderr,"trying to watch a file already under watch: %s\n", fname);
            return -1;
        }
    };


    // if we are here, the file has not been registered yet.
    file_watcher_entry entry = {
        .is_valid = 1,
        .cb = cb
    };

    if (!file_get_stats(fname,&entry.filestats)) {
        fprintf(stderr, "cannot stat file %s\n", fname);
        return -1;
    };

    strncpy(entry.filename,fname,strlen(fname)); 
    ctx->file_entries[ctx->entry_count] = entry; // always append
    ctx->entry_count++;
    return 0;
}

void fw_stop_watch(struct file_watcher_ctx* ctx, const char* fname)
{
    for (int i = 0; i < ctx->entry_count; i++) {
        if (strcmp(ctx->file_entries[i].filename, fname) == 0) {
            ctx->file_entries[i].is_valid = 0;
            break;
        }
    }
    return;
}

void fw_shutdown(struct file_watcher_ctx* ctx)
{
    // this is supposed to be atomic...
    g_stop_thread_flag = 1;
    pthread_join(watcher_thread, NULL);
    return;
}

