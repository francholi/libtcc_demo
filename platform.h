#include <stdbool.h> 

#define INPUT_KEY_ESCAPE GLFW_KEY_ESCAPE

#define INPUT_KEY_PRESS GLFW_PRESS
#define INPUT_KEY_RELEASE GLFW_RELEASE
#define INPUT_INVALID_KEY 2


struct window_handle;

void window_init(int width, int height, const char* title);
void window_destroy();
bool window_should_close();
void window_swap_buffers();

void input_poll();
bool input_key_pressed(int key);
bool input_key_released(int key);

// char input_get_key(char key);
// Window_Handle* window_get_platform_handle();




