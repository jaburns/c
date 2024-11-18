#pragma once

structdef(KeyboardState) {
    bool scancodes_down[SDL_NUM_SCANCODES];
};

#define JOYSTICK_AXIS_COUNT   32
#define JOYSTICK_BUTTON_COUNT 32

structdef(JoystickState) {
    f32  axis_values[JOYSTICK_AXIS_COUNT];
    bool buttons_down[JOYSTICK_BUTTON_COUNT];
};

structdef(SdlGlWindow) {
    SDL_Window*       sdl_window;
    SDL_GLContext     sdl_context;
    SDL_AudioDeviceID sdl_audio_device;
    AudioCallbackFn   audio_callback_fn;
    AudioPlayer       audio_player;
#if DEBUG
    ImGuiContext* imgui_context;
#endif
    SDL_JoystickID active_joystick_id;
    SDL_Joystick*  sdl_joystick;
    JoystickState  joystick;
    KeyboardState  keyboard;
    ivec2          screen_size;
    ivec2          mouse_delta;
    f32            mouse_delta_wheel;
    bool           mouse_button;
};

internal void sdlgl_create(SdlGlWindow* win, char* window_title, SDL_AudioCallback sdl_audio_callback);
internal i32  sdlgl_poll(SdlGlWindow* win);
internal void sdlgl_swap(SdlGlWindow* win);