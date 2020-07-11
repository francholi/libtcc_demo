#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "file_utils.h"
#include "file_watcher.h"

int syncfs(int fd);

bool file_changed(const char* filename, struct stat* old_new_stat) {
    struct stat current_stat;
    bool res = file_get_stats(filename, &current_stat);
    if (res == true)
    {
        // has file modification time changed?
        if (difftime(current_stat.st_mtime, old_new_stat->st_mtime) != 0) 
        {
            // update file stamp
            *old_new_stat = current_stat;
            return true;
        }
        else 
        {
            return false;
        }
    }
};

size_t file_get_contents(const char* filename, char* dest, size_t size)
{
    if (size == 0) {
        size = file_get_size(filename);
    }

    FILE* fd = fopen(filename, "r");
    if (fd == NULL) {
        perror("Error opening file:");
        return 0;
    }
    syncfs(fileno(fd)); // only linux!

    size_t read_bytes = fread(dest, 1, size, fd);
    /*
    if (read_bytes != size)
    {
        fprintf(stderr, "Error reading file %s, not all bytes were read\n", filename);
	fprintf(stderr, "Read: %d; Expected: %d\n", read_bytes, size);
        return 0;
    }
    */
    dest[read_bytes - 1] = '\0';
    fclose(fd);
    return read_bytes;
}

size_t file_get_size(const char* filename)
{
    struct stat temp_stat;
    if (file_get_stats(filename, &temp_stat))
        return temp_stat.st_size;
    else
        return 0;
}

bool file_get_stats(const char* filename, struct stat* file_stat)
{
    struct stat temp_file_stat;
    int s = stat(filename, &temp_file_stat);
    if (s == -1) 
    {
        fprintf(stderr, "stat error, file does not exist!", filename);
        return false;
    };
    *file_stat = temp_file_stat;
    return true;
}

