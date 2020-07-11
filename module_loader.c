#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "module_loader.h"
#include "file_watcher.h"
#include "file_utils.h"
#include "shader.h"

extern GlobalState* gs;

int syncfs(int fd);

// initialised to 0.
#define MAX_SYMBOL 200
#define MAX_FILE_NAMES 100

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
    char* filename;
    struct stat file_stats;
    SymbolMap* symbol_maps[MAX_SYMBOL];
} FileEntry;

// hardcoded number of files, and ofc super inneficient for many files.
// just a proof of concept.
static FileEntry* gFile_entries[MAX_FILE_NAMES];
static int g_Last_added_file_entry;

// not thread-safe!!
// callback for compilation errors.
static void tcc_err_callback(void* err_opaque, const char* msg)
{
    fprintf(stderr, "%s\n", msg);
    fflush(NULL);
};

/*
 * Not thread safe.
 * register filename and symbol, so that when reloading the code we can 
 * get a new pointer to the newly loaded function.
 * Another option would be to gather ALL symbols with some C parser, and reload them
 * all.
 * Store a double pointer to fix the original function pointer of the caller of this
 * function.
 * This function ONLY registers a file and a pointer to function, does not compile
 * or do anything else.
 * If called TWICE with the same "name" and different pointer, it will ignore!!
 */
int register_symbol(const char* symbol_name, void** func_pointer, const char* filename, char hotreload) {

    //if (hotreload == 0)
    //    return 0;

    // linear search. find filename.
    int index_file = 0;
    while (index_file < g_Last_added_file_entry && strcmp( gFile_entries[index_file]->filename, filename) != 0) 
    {
        index_file++;
    }

    // register new file entry if file was not found.
    if (index_file == g_Last_added_file_entry) 
    {
        FileEntry* f = malloc(sizeof(FileEntry)); // TODO: leaking this memory, once per file?
        memset(f, 0, sizeof(FileEntry));
        f->filename = malloc(strlen(filename));
        strcpy( f->filename, filename );
        if (f->file_stats.st_size == 0)
            file_get_stats(filename, &f->file_stats);

        gFile_entries[index_file] = f;
        
        g_Last_added_file_entry++;
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
        SymbolMap* mymap = malloc(sizeof(SymbolMap)); // TODO: leaking again
        mymap->function_name = malloc(strlen(symbol_name));
        strcpy( mymap->function_name, symbol_name);
        mymap->function_pointer = func_pointer;

        gFile_entries[index_file]->symbol_maps[index_symbol] = mymap;
    }
    return 0;
};

// create a new TCCState, caller MUST release the memory if the
// return value is not NULL
TCCState* compile_module(const char* source)
{
    TCCState* newState = tcc_new();
    tcc_set_error_func(newState, NULL, tcc_err_callback); // error callback 
    tcc_set_output_type(newState, TCC_OUTPUT_MEMORY);     // compilation result to memory
    int compile_res = tcc_compile_string(newState, source);
    if (compile_res == -1)
    {
        tcc_delete(newState);
        return NULL; // error compiling
    }
    return newState;
};

#define RELOAD_ERR -1
#define RELOAD_OK   0
int reload_symbols(const char* filename) { 
    // - compile, add symbols, try to relocate.
    // - update function pointers and TCC_State to a new state,
    // - destroy previous TCC_State

    int file_index = 0;
    while (file_index < MAX_FILE_NAMES && gFile_entries[file_index] != NULL) {

        // a working version of the code.
        FileEntry* entry = gFile_entries[file_index];

        // only reload for this filename
        if (strcmp( entry->filename, filename)!=0) {
            file_index++;
            continue;
        }

        // if file didnt change, this should not be called!
        //if (file_changed(entry->filename, &entry->file_stats) == false)
        //{
        //    file_index++;
        //    return RELOAD_ERR;
        //}

        // assuming file has changed, otherwise we wouldnt be calling this func
        // read file contents
        char *file_content = malloc(entry->file_stats.st_size);
        size_t read = file_get_contents(
            entry->filename,
            file_content,
            entry->file_stats.st_size);

        // try to recompile changed module
        TCCState *newState = compile_module(file_content);
        if (newState == NULL)
        {
            // did not compile!
            free(file_content);
            // TODO: how to report the error
            return RELOAD_ERR;
        }

        TCCState *oldState = 0;

        // first time we try to recompile this module??
        if (entry->state != NULL)
            oldState = entry->state;

        entry->state = newState;

        // globalState passed this way seems like a nice alternative to
        // restore global state in the dynamic code. From here I will save
        // and reload "stuff"
        tcc_add_symbol(entry->state, "gs", &gs);

        // Relocate can fail for instance if we have an invalid symbol
        // like a function mispelled
        if (tcc_relocate(entry->state, TCC_RELOCATE_AUTO) == -1)
        {
            entry->state = oldState;
            tcc_delete(newState);
            fprintf(stderr, "relocate failed\n");
            free(file_content);
            fflush(NULL);
            return RELOAD_ERR;
        }

        // here we have succedeed, so delete old state.
        if (oldState!=NULL)
            tcc_delete(oldState);
        free(file_content);
        file_index++;
    }
    return RELOAD_OK;
};

// for a given filename, go through each registered symbol and
// get the new pointers based on the last tcc_state
// 0 is ok results
// -1 is not ok
int patch_pointers(const char* filename) {
    int file_index = 0;
    while ( file_index < MAX_FILE_NAMES && 
            gFile_entries[file_index] != NULL) {

        // a working version of the code.
        FileEntry* entry = gFile_entries[file_index];

        // only patch for this filename
        if (strcmp( entry->filename, filename)!=0) {
            file_index++;
            continue;
        }

        // point of no return!, if we reach this point we exit the loop!
        int index_symbol = 0;
        while (entry->symbol_maps[index_symbol] != NULL)
        {
            // patch symbols
            *(entry->symbol_maps[index_symbol]->function_pointer) =
                tcc_get_symbol(
                    entry->state,
                    entry->symbol_maps[index_symbol]->function_name);
            index_symbol++;
        }
        return 0;
    };
    return -1;
};
