#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include "file_utils.h"
#include "file_watcher.h"

//typedef void (*callback)(const char*      ); // func. pointer type named "callback"

// keep track of files, timestamps, frequency of check, and a callback per file.
typedef struct file_watcher_entry {
    bool         is_valid;
    char         filename[255];
    struct stat  filestats;
    unsigned int check_freq; // in milliseconds
    callback cb;
} file_watcher_entry;

typedef struct file_watcher_ctx 
{
  file_watcher_entry file_entries[255];
  unsigned int entry_count;
} file_watcher_ctx;

static file_watcher_ctx g_context;

struct file_watcher_ctx* fw_init()
{

    g_context.entry_count = 0;

    // create thread that will check at regular intervals all the files for changes.
    // create thread, launch thread for 500 ms intervals;
    
    return &g_context;
}

// return 0 on sucess, -1 if any error 
int fw_watch(struct file_watcher_ctx* ctx, const char* fname, callback cb, unsigned int freq_ms)
{
    // check all the names in the ctx, if it is registered already, ignore.;
    // if is a new filename, register a new entry, which the thread has to loop through.
    // (beware contention here!, fw_watch is called from the main thread!)

    // check if the file is already registered.
    for (int i = 0; i < ctx->entry_count; i++)
    {
        // for now, to deal with holes
        if (!ctx->file_entries[i].is_valid)
            continue;

        if (strcmp(ctx->file_entries[i].filename, fname) == 0)
        {
            fprintf(stderr,"trying to watch a file already under watch: %s\n", fname);
            return -1;
        }
    };

    // if we are here, the file has not been registered yet.
    file_watcher_entry entry = {
        .is_valid = true,
        .check_freq = freq_ms,
        .cb = cb
    };

    if (!file_get_stats(fname,&entry.filestats)) {
        fprintf(stderr, "cannot stat file %s\n", fname);
        return -1;
    };

    strncpy(entry.filename,fname,strlen(fname)); 
    ctx->file_entries[ctx->entry_count] = entry;
    ctx->entry_count++;
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

