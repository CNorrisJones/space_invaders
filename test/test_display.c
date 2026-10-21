#include <stdlib.h>
#include <stdio.h>

#include "display.h"
#include "input_state.h"

int main(void) {
    DisplayContext *display = DisplayInit(256, 224);
    if (display == NULL) {
        printf("Error initializing the DisplayContext\n");
        return DISPLAY_FAILURE;
    }

    uint32_t *frame_buffer = malloc(256 * 224 * sizeof(uint32_t));

    if (frame_buffer == NULL) {
        printf("Error initializing the frame_buffer\n");
        DisplayShutdown(display);
        return DISPLAY_FAILURE;
    }
    
    // Set element in frame_buffer to black pixel
    for (int y = 0; y < 224; y++) {
        for (int x = 0; x < 256; x++) {
            if ((x == 0) || (y == 0) || (x == 1) || (y == 1)) {
                frame_buffer[y * 256 + x] = display->pixel_palette.white_pixel;
            } else {
                frame_buffer[y * 256 + x] = display->pixel_palette.black_pixel;
            }
        }
    }

    InputState input = {0};

    typedef struct KeyBindingTest {
        SDL_Keycode key;
        bool *control;
    } KeyBindingTest;

    KeyBindingTest bindings[] = {
        { SDLK_a, &input.p1_left },
        { SDLK_d, &input.p1_right },
        { SDLK_w, &input.p1_fire },
        { SDLK_LEFT, &input.p2_left },
        { SDLK_RIGHT, &input.p2_right },
        { SDLK_UP, &input.p2_fire },
        { SDLK_RETURN, &input.credit },
        { SDLK_1, &input.one_player },
        { SDLK_2, &input.two_player },
        { SDLK_TAB, &input.tilt }
    };

    size_t binding_count = sizeof(bindings) / sizeof(bindings[0]);
    SDL_Event event;
    SDL_zero(event);
    int check_val;

    for (size_t i = 0; i < binding_count; ++i) {
        KeyBindingTest *binding = &bindings[i];

        event.type = SDL_KEYDOWN;
        event.key.state = SDL_PRESSED;
        event.key.repeat = 0;
        event.key.keysym.sym = binding->key;

        check_val = SDL_PushEvent(&event);
        if (check_val != 1) {
            printf("Error Pushing Test Event Key Down to Queue\n");
            free(frame_buffer);
            DisplayShutdown(display);
            return DISPLAY_FAILURE;
        }

        check_val = DisplayPollEvents(&input);
        if (check_val != DISPLAY_CONTINUE) {
            printf("Error Displaying Poll Event For Key Down to Queue\n");
            free(frame_buffer);
            DisplayShutdown(display);
            return DISPLAY_FAILURE;
        }
        if (*binding->control != true) {
            printf("Failure Confirming Key Down Pushed Down\n");
            free(frame_buffer);
            DisplayShutdown(display);
            return DISPLAY_FAILURE;
        }

        for (size_t j = 0; j < binding_count; ++j) {
            if (i == j) {
                continue;
            }
            KeyBindingTest *false_binding = &bindings[j];
            if (*false_binding->control != false) {
                printf("Unexpected Key Value Returned True\n");
                free(frame_buffer);
                DisplayShutdown(display);
                return DISPLAY_FAILURE;
            }
        }

        event.type = SDL_KEYUP;
        event.key.state = SDL_RELEASED;
        check_val = SDL_PushEvent(&event);
        if (check_val != 1) {
            printf("Error Pushing Test Event Key Up to Queue\n");
            free(frame_buffer);
            DisplayShutdown(display);
            return DISPLAY_FAILURE;
        }

        check_val = DisplayPollEvents(&input);
        if (check_val != DISPLAY_CONTINUE) {
            printf("Error Displaying Poll Event For Key Up to Queue\n");
            free(frame_buffer);
            DisplayShutdown(display);
            return DISPLAY_FAILURE;
        }

        for (size_t j = 0; j < binding_count; ++j) {
            KeyBindingTest *false_binding = &bindings[j];
            if (*false_binding->control != false) {
                printf("Unexpected Key Value Returned False\n");
                free(frame_buffer);
                DisplayShutdown(display);
                return DISPLAY_FAILURE;
            }
        }
    }

    event.type = SDL_KEYDOWN;
    event.key.state = SDL_PRESSED;
    event.key.keysym.sym = SDLK_a;
    check_val = SDL_PushEvent(&event);
    if (check_val != 1) {
        printf("Error Displaying Poll Event For Simul Key Down to Queue\n");
        free(frame_buffer);
        DisplayShutdown(display);
        return DISPLAY_FAILURE;
    }
    event.key.keysym.sym = SDLK_w;
    check_val = SDL_PushEvent(&event);
    if (check_val != 1) {
        printf("Error Displaying Poll Event For Simul Key Down to Queue\n");
        free(frame_buffer);
        DisplayShutdown(display);
        return DISPLAY_FAILURE;
    }

    check_val = DisplayPollEvents(&input);
    if (check_val != DISPLAY_CONTINUE) {
        printf("Error Displaying Poll Event For Simul Key Down to Queue\n");
        free(frame_buffer);
        DisplayShutdown(display);
        return DISPLAY_FAILURE;
    }

    for (size_t i = 0; i < binding_count; ++i) {
        KeyBindingTest *binding = &bindings[i];
        if (binding->key == SDLK_a || binding->key == SDLK_w) {
            if (*binding->control != true) {
                printf("Simul Key Left and Fire Returned False\n");
                free(frame_buffer);
                DisplayShutdown(display);
                return DISPLAY_FAILURE;
            }
        } else {
            if (*binding->control != false) {
                printf("Simul Key Press Returned True for Unexpected Input\n");
                free(frame_buffer);
                DisplayShutdown(display);
                return DISPLAY_FAILURE;
            }
        }
    }

    event.type = SDL_KEYUP;
    event.key.state = SDL_RELEASED;
    event.key.keysym.sym = SDLK_a;
    check_val = SDL_PushEvent(&event);
    if (check_val != 1) {
        printf("Error Displaying Poll Event For Simul Key Up to Queue\n");
        free(frame_buffer);
        DisplayShutdown(display);
        return DISPLAY_FAILURE;
    }

    check_val = DisplayPollEvents(&input);
    if (check_val != DISPLAY_CONTINUE) {
        printf("Error Displaying Poll Event For Simul Key Up to Queue\n");
        free(frame_buffer);
        DisplayShutdown(display);
        return DISPLAY_FAILURE;
    }

    for (size_t i = 0; i < binding_count; ++i) {
        KeyBindingTest *binding = &bindings[i];
        if (binding->key == SDLK_w) {
            if (*binding->control != true) {
                printf("Simul Key Fire Returned False\n");
                free(frame_buffer);
                DisplayShutdown(display);
                return DISPLAY_FAILURE;
            }
        } else {
            if (*binding->control != false) {
                printf("Simul Key Press Returned True for Unexpected Input\n");
                free(frame_buffer);
                DisplayShutdown(display);
                return DISPLAY_FAILURE;
            }
        }
    }

    event.key.keysym.sym = SDLK_w;
    check_val = SDL_PushEvent(&event);
    if (check_val != 1) {
        printf("Error Displaying Poll Event For Simul Key Up to Queue\n");
        free(frame_buffer);
        DisplayShutdown(display);
        return DISPLAY_FAILURE;
    }

    check_val = DisplayPollEvents(&input);
    if (check_val != DISPLAY_CONTINUE) {
        printf("Error Displaying Poll Event For Simul Key Up to Queue\n");
        free(frame_buffer);
        DisplayShutdown(display);
        return DISPLAY_FAILURE;
    }

    for (size_t i = 0; i < binding_count; ++i) {
        KeyBindingTest *binding = &bindings[i];
        if (*binding->control != false) {
            printf("Simul Key Press Returned True for Unexpected Input\n");
            free(frame_buffer);
            DisplayShutdown(display);
            return DISPLAY_FAILURE;
        }
    }

    int quit = 0;
    while(!quit) {
        quit = DisplayPollEvents(&input);

        int frame_status = DisplayFrame(display, frame_buffer);
        if (frame_status != 0) {
            printf("Error setting up frame status\n");
            free(frame_buffer);
            DisplayShutdown(display);
            return DISPLAY_FAILURE;
        }

        // Optionally Delay Briefly
        SDL_Delay(16);
    }

    printf("Display Test Successful!\n");
    free(frame_buffer);
    DisplayShutdown(display);

    return DISPLAY_SUCCESS;
}
