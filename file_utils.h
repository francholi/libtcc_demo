#include <stddef.h>
#include <stdbool.h>
struct stat;

bool file_get_stats(const char* filename, struct stat* file_stat);
size_t      file_get_size(const char* filename);
// if size == 0, the function will retrieve the size internally
size_t      file_get_contents(const char* filename, char* dest, size_t size);
bool file_changed(const char* filename, struct stat* old_new_stat);

