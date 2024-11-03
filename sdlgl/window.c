#include "inc.h"

internal void sdlgl_create(SdlGlWindow* mw, char* window_title, SDL_AudioCallback sdl_audio_callback) {
    ZERO_STRUCT(mw);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0) {
        PANIC("Failed to initialize SDL: %s\n", SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    mw->screen_size.x = 1280;
    mw->screen_size.y = 720;

    mw->sdl_window = SDL_CreateWindow(
        window_title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        mw->screen_size.x, mw->screen_size.y,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (!mw->sdl_window) {
        PANIC("Failed to create SDL window: %s\n", SDL_GetError());
    }

    mw->sdl_context = SDL_GL_CreateContext(mw->sdl_window);
    if (!mw->sdl_context) {
        PANIC("Failed to create SDL GL context: %s\n", SDL_GetError());
    }

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        PANIC("Error initializing GLEW: %s\n", glewGetErrorString(err));
    }

    if (SDL_GL_SetSwapInterval(1) < 0) {
        PANIC("Unable to set VSync: %s", SDL_GetError());
    }

#if DEBUG
    mw->imgui_context = igCreateContext(NULL);
    ImGuiIO* io = igGetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplSdlGL3_Init(mw->sdl_window, NULL);
#endif

    SDL_AudioSpec obtained_spec;
    SDL_AudioSpec desired_spec = (SDL_AudioSpec){
        .freq = AUDIO_SAMPLE_RATE,
        .format = AUDIO_F32,
        .channels = 2,
        .samples = 512,
        .callback = sdl_audio_callback,
        .userdata = mw,
    };

    mw->sdl_audio_device = SDL_OpenAudioDevice(NULL, 0, &desired_spec, &obtained_spec, 0);
    if (mw->sdl_audio_device == 0) {
        PANIC("Failed to open audio device: %s\n", SDL_GetError());
    }
    SDL_PauseAudioDevice(mw->sdl_audio_device, 0);

    for (u32 i = 0; i < SDL_NumJoysticks(); ++i) {
        printf("Using joystick: %s\n", SDL_JoystickNameForIndex(i));
        mw->sdl_joystick = SDL_JoystickOpen(i);
        mw->active_joystick_id = i;
        break;
    }
}

internal i32 sdlgl_poll(SdlGlWindow* mw) {
    mw->mouse_delta = (ivec2){0};
    mw->mouse_delta_wheel = 0.f;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
#if DEBUG
        ImGui_ImplSdlGL3_ProcessEvent(&event);
#endif
        switch (event.type) {
            case SDL_WINDOWEVENT: {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED: {
                        i32 w = event.window.data1;
                        i32 h = event.window.data2;
                        mw->screen_size.x = w;
                        mw->screen_size.y = h;
                        break;
                    }
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                mw->mouse_button = true;
                break;
            }
            case SDL_MOUSEBUTTONUP: {
                mw->mouse_button = false;
                break;
            }
            case SDL_MOUSEMOTION: {
                mw->mouse_delta.x = event.motion.xrel;
                mw->mouse_delta.y = -event.motion.yrel;
                break;
            }
            case SDL_MOUSEWHEEL: {
                mw->mouse_delta_wheel = event.wheel.preciseY;
                break;
            }
            case SDL_QUIT: {
                return 1;
            }
            case SDL_KEYDOWN: {
                if (event.key.keysym.scancode == SDL_SCANCODE_Q)
                    return 1;

                mw->keyboard.scancodes_down[event.key.keysym.scancode] = true;
                break;
            }
            case SDL_KEYUP: {
                mw->keyboard.scancodes_down[event.key.keysym.scancode] = false;
                break;
            }
            case SDL_JOYAXISMOTION: {
                // printf("Joystick %d axis %d moved to %d\n", event.jaxis.which, event.jaxis.axis, event.jaxis.value);
                if (event.jaxis.which == mw->active_joystick_id && event.jaxis.axis < JOYSTICK_AXIS_COUNT) {
                    mw->joystick.axis_values[event.jaxis.axis] =
                        event.jaxis.value < 0 ? (f32)event.jaxis.value / 32768.0 : (f32)event.jaxis.value / 32767.0;
                }
                break;
            }
            case SDL_JOYBUTTONDOWN: {
                // printf("Joystick %d button %d pressed\n", event.jbutton.which, event.jbutton.button);
                if (event.jbutton.which == mw->active_joystick_id && event.jbutton.button < JOYSTICK_BUTTON_COUNT) {
                    mw->joystick.buttons_down[event.jbutton.button] = true;
                }
                break;
            }
            case SDL_JOYBUTTONUP: {
                // printf("Joystick %d button %d released\n", event.jbutton.which, event.jbutton.button);
                if (event.jbutton.which == mw->active_joystick_id && event.jbutton.button < JOYSTICK_BUTTON_COUNT) {
                    mw->joystick.buttons_down[event.jbutton.button] = false;
                }
                break;
            }
        }
    }
#if DEBUG
    ImGui_ImplSdlGL3_NewFrame(mw->sdl_window);
#endif
    return 0;
}

internal void sdlgl_swap(SdlGlWindow* mw) {
#if DEBUG
    igRender();
    ImGui_ImplSdlGL3_RenderDrawData(igGetDrawData());
#endif
    SDL_GL_SwapWindow(mw->sdl_window);
}