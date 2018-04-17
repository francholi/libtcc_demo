#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include "module_loader.h"

#include "shader.h"

extern GlobalState* gs;

// initialised to 0.
#define MAX_SYMBOL 200
#define MAX_FILE_NAMES 50

// keep track of symbol names and symbols pointers.
typedef struct SymbolMap {
  void** function_pointer;
  char* function_name;
} SymbolMap;

// keep track of a compiled module.
// TCCState is for one MODULE only, and can deal with allocations
// for compiled code, so need to keep around whilst using the code.
// If recompiled, state must be destroyed and recreated.
typedef struct FileEntry {
  TCCState* state; // this must be kept around in memory.
  char* file_name;
  time_t modified_time;
  SymbolMap* symbol_maps[MAX_SYMBOL];
} FileEntry;

// hardcoded number of files, and ofc super inneficient for many files.
// just a proof of concept.
static FileEntry* gFile_entries[MAX_FILE_NAMES];
static int gLast_file_entry;

// not thread-safe!!
// callback for compilation errors.
static void tcc_err_callback(void* err_opaque, const char* msg)
{
  fprintf(stderr, "%s\n", msg);
};

/*
 * Not thread safe.
 * register filename and symbol, so that when reloading the code we can 
 * get a new pointer to the newly loaded function.
 * Store a double pointer to fix the original function pointer of the caller of this
 * function.
 * This function ONLY registers a file and a pointer to function, does not compile
 * or do anything else.
 * If called TWICE with the same "name" and different pointer, it will ignore!!
 */
int watch_symbol(const char* symbol_name, void** func_pointer, const char* file_name) {

  // linear search. find filename.
  int index_file = 0;
  while (index_file < gLast_file_entry && 
         strcmp( gFile_entries[index_file]->file_name, file_name) != 0) 
  {
         index_file++;
  }

  // register new file entry if file was not found.
  if (index_file == gLast_file_entry) 
  {
    FileEntry* f = malloc(sizeof(FileEntry));
    memset(f, 0, sizeof(FileEntry));
    f->file_name = malloc(strlen(file_name));
    strcpy( f->file_name, file_name );
    gFile_entries[index_file] = f;
    gLast_file_entry++;
  };

  // check if symbol is already registered (!=NULL)
  int index_symbol = 0;
  while ( gFile_entries[index_file]->symbol_maps[index_symbol]!=NULL &&
          strcmp( gFile_entries[index_file]->symbol_maps[index_symbol]->function_name, symbol_name ) != 0) 
  {
    index_symbol++;
  }

  // save symbol name and pointer to function pointer
  if (gFile_entries[index_file]->symbol_maps[index_symbol] == NULL) 
  {
    SymbolMap* mymap = malloc(sizeof(SymbolMap));
    mymap->function_name = malloc(strlen(symbol_name));
    strcpy( mymap->function_name, symbol_name);
    mymap->function_pointer = func_pointer;

    gFile_entries[index_file]->symbol_maps[index_symbol] = mymap;
  }
  return 0;
};


/*
 * every_N_frame is the lame/lazy way..., a timer would be better :)
 */
void reload_symbols(int every_N_frame) { 
  static long long counter = 0;
  if (counter % every_N_frame != 0)
    return;

  // for each file registered
  // - compile the file if changed.
  // - if file has changed, 
  //   - add global state symbol
  //   - add any other symbol needed by loaded file (hardcoded for now)
  //   - get function pointers and match them.
  int file_index = 0;
  while (file_index < MAX_FILE_NAMES && gFile_entries[file_index] != NULL) {

    // a working version of the code.
    FileEntry* entry = gFile_entries[file_index];

    struct stat this_file_stat;
    int s = stat(entry->file_name, &this_file_stat);
    if (s == -1) {
      fprintf(stderr, "ERROR; file does not exist: %s", entry->file_name);
      file_index++;
      continue;
    };
    // file has changed, or never compiled before.
    if (difftime(this_file_stat.st_mtime, entry->modified_time) != 0) {
      // update file stamp
      entry->modified_time = this_file_stat.st_mtime;


      // read file contents
      // not sure about this, but I have had issues when I save the file whilst
      // trying to open it for reading (giving partial reads), so flushing IO has
      // improved a bit.
      fflush(NULL);
      // this malloc could be done outside once, if no need for concurrent calls.
      char* file_content = malloc(this_file_stat.st_size);
      FILE* source_file = fopen(entry->file_name, "r");
      size_t read_bytes = fread(file_content, 1, this_file_stat.st_size, source_file);
      file_content[read_bytes - 1] = '\0';
      fclose(source_file);

      // make a new state to TRY and compile the changes, otherwise dispose and
      // keep working copy
      TCCState* newState = tcc_new();
      TCCState* oldState = 0;

      // prepare to compile TO MEMORY, use the newState
      tcc_set_error_func(newState, NULL, tcc_err_callback);  
      // compile directly to memory
      tcc_set_output_type(newState, TCC_OUTPUT_MEMORY);
      if (tcc_compile_string(newState, file_content) != -1) {
        // if we managed to compile, destroy previous state,
        // use the new one. (can be null, if first time, hence the if)
        if (entry->state)
           oldState = entry->state;

        entry->state = newState;

        // LIBTCC MAGIC HERE.
        // did not time this, but it is really fast.

        // globalState passed this way seems like a nice alternative to
        // restore global state in the dynamic code.
        tcc_add_symbol(entry->state, "globalState", &gs);
        // this I don't like, I would like a better way to define what symbols
        // to add per module basis, not here.
        tcc_add_symbol(entry->state, "recompile_program", recompile_program);

        tcc_relocate(entry->state, TCC_RELOCATE_AUTO);
        // remap pointers to functions from the watch list.
        int index_symbol = 0;
        while (entry->symbol_maps[index_symbol] != NULL) {
          *(entry->symbol_maps[index_symbol]->function_pointer) =
              tcc_get_symbol(entry->state, entry->symbol_maps[index_symbol]->function_name);
          index_symbol++;
        };

        // Gracefully remove the current module. 
        // UNLOAD + RELOAD FILE
        // Executes unload_filename(), then load_filename()
        // first unload.
        char function_name[255];
        memset(function_name, 0, 255);
        sprintf(function_name, "unload_%s", entry->file_name);
        // force null termination at the colon extension separator to remove ".c"
        strstr(function_name, ".")[0] = '\0';
        int (*unload_function_ptr)(void) = tcc_get_symbol(entry->state, function_name);
        if (unload_function_ptr != NULL) {
          unload_function_ptr();
        };

        // now load again
        int (*load_function_ptr)(void) =
            tcc_get_symbol(entry->state, function_name + 2);  // unload->[un]load

        if (load_function_ptr != NULL) {
          load_function_ptr();
        }

        // all worked, delete old state (old state can be NULL the first time)
        if (oldState)
          tcc_delete(oldState);
      }
      else {
        // compilation failed!, delete new state
        fflush(NULL);
        tcc_delete(newState);
      };
      free(file_content);
    };
    file_index++;
  };
  return;
};

