#include "libtcc.h>
#include "global_state_def.h"
#include "driver.h"


extern GlobalState* gs;

void load_driver(notify_reload_callback callback)
{
    // [] register myself into the file watcher,
    //    to get notified when I am changed. This
    //    notification will come from a separate thread!
    //
    // [] setup a callback function for the file_watcher.
    //
    // [] in the callback, call notify_reload_callback 
    //    from main, to let main know that we have been
    //    modified and would like to try to recompile.
    //
    // [] main receives call about this, tries to recompile in
    //    memory the new code, if it succedees, calls unload_driver
    //    to release resources, replaces the compiled code, and
    //    calls load_driver again to start again the cycle.
    // 
}
void unload_driver(void) 
{

}

void start(void)
{

}

