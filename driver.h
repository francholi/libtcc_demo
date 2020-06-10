typedef void (*notify_reload_callback)(void*);

// this function is called by main.cpp right after this module has been
// loaded, and passes a function ptr so that when it is time to reload
// because there was a change in the file (driver.c) we let main know
// and main will try to recompile the new code, and if it succedees it
// will call unload_driver, the replace the code, call load_driver again
// with the new instance.
void load_driver(notify_reload_callback callback);
void unload_driver(void);
void start(void);

