#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL_mixer.h>

#include "audio.h"

static const int FREQUENCY = 44100;
static const int CHANNELS = 2;
static const int AUDIO_BUFFER_SIZE = 2048;
static const int DISABLED_UFO_MOVE_CHANNEL = -1;
static const bool ACTIVATE_UFO_MOVE = true;

struct AudioContext {
    Mix_Chunk   *invader_killed;
    Mix_Chunk   *invader_move_1;
    Mix_Chunk   *invader_move_2;
    Mix_Chunk   *invader_move_3;
    Mix_Chunk   *invader_move_4;
    Mix_Chunk   *player_killed;
    Mix_Chunk   *player_shoot;
    Mix_Chunk   *ufo_killed;
    Mix_Chunk   *ufo_move;
    bool        mixer_open;
    int         ufo_move_channel;
};

AudioContext *AudioInit(void) {
    AudioContext *audio_context = malloc(sizeof(AudioContext));
    if (audio_context == NULL) {
        return NULL;
    }

    //The sound effects that will be used
    audio_context->invader_killed = NULL;
    audio_context->invader_move_1 = NULL;
    audio_context->invader_move_2 = NULL;
    audio_context->invader_move_3 = NULL;
    audio_context->invader_move_4 = NULL;
    audio_context->player_killed = NULL;
    audio_context->player_shoot = NULL;
    audio_context->ufo_killed = NULL;
    audio_context->ufo_move = NULL;
    audio_context->ufo_move_channel = DISABLED_UFO_MOVE_CHANNEL;
    audio_context->mixer_open = false;

    //Initialize SDL_mixer
    int audio_init = Mix_OpenAudio(FREQUENCY, MIX_DEFAULT_FORMAT, CHANNELS, AUDIO_BUFFER_SIZE);
    if (audio_init == AUDIO_FAILURE) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        AudioShutdown(audio_context);
        return NULL;
    }
    audio_context->mixer_open = true;

    audio_context->invader_killed = Mix_LoadWAV("sound/invader_killed.wav");
    if(audio_context->invader_killed == NULL) {
        AudioShutdown(audio_context);
        return NULL;
    }

    audio_context->invader_move_1 = Mix_LoadWAV("sound/invader_move_1.wav");
    if(audio_context->invader_move_1 == NULL) {
        AudioShutdown(audio_context);
        return NULL;
    }

    audio_context->invader_move_2 = Mix_LoadWAV("sound/invader_move_2.wav");
    if(audio_context->invader_move_2 == NULL) {
        AudioShutdown(audio_context);
        return NULL;
    }

    audio_context->invader_move_3 = Mix_LoadWAV("sound/invader_move_3.wav");
    if(audio_context->invader_move_3 == NULL) {
        AudioShutdown(audio_context);
        return NULL;
    }

    audio_context->invader_move_4 = Mix_LoadWAV("sound/invader_move_4.wav");
    if(audio_context->invader_move_4 == NULL) {
        AudioShutdown(audio_context);
        return NULL;
    }

    audio_context->player_killed = Mix_LoadWAV("sound/player_killed.wav");
    if(audio_context->player_killed == NULL) {
        AudioShutdown(audio_context);
        return NULL;
    }

    audio_context->player_shoot = Mix_LoadWAV("sound/player_shoot.wav");
    if(audio_context->player_shoot == NULL) {
        AudioShutdown(audio_context);
        return NULL;
    }

    audio_context->ufo_killed = Mix_LoadWAV("sound/ufo_killed.wav");
    if(audio_context->ufo_killed == NULL) {
        AudioShutdown(audio_context);
        return NULL;
    }

    audio_context->ufo_move = Mix_LoadWAV("sound/ufo_move.wav");
    if(audio_context->ufo_move == NULL) {
        AudioShutdown(audio_context);
        return NULL;
    }

    return audio_context;
}

int AudioSetUfoMovement(AudioContext *audio_context, bool activate) {
    if (audio_context == NULL) {
        return AUDIO_FAILURE;
    }

    int audio_channel;

    if (activate) {
        if (audio_context->ufo_move_channel != DISABLED_UFO_MOVE_CHANNEL) {
            return AUDIO_SUCCESS;
        }

        audio_context->ufo_move_channel = Mix_PlayChannel(-1, audio_context->ufo_move, -1);
        if (audio_context->ufo_move_channel == AUDIO_FAILURE) {
            return AUDIO_FAILURE;
        }
    } else {
        if(audio_context->ufo_move_channel == DISABLED_UFO_MOVE_CHANNEL) {
            return AUDIO_SUCCESS;
        }

        audio_channel = Mix_HaltChannel(audio_context->ufo_move_channel);
        if (audio_channel == AUDIO_FAILURE) {
            return AUDIO_FAILURE;
        }
        audio_context->ufo_move_channel = DISABLED_UFO_MOVE_CHANNEL;
    }
    return AUDIO_SUCCESS;
}

int PlayAudio(AudioContext *audio_context, int audio_flag) {
    if (audio_context == NULL) {
        return AUDIO_FAILURE;
    }

    int audio_channel;

    switch(audio_flag) {
        case(INVADER_KILLED):
            audio_channel = Mix_PlayChannel(-1, audio_context->invader_killed, 0);
            if (audio_channel == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }

            break;
        case(INVADER_MOVE_1):
            audio_channel = Mix_PlayChannel(-1, audio_context->invader_move_1, 0);
            if (audio_channel == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }

            break;
        case(INVADER_MOVE_2):
            audio_channel = Mix_PlayChannel(-1, audio_context->invader_move_2, 0);
            if (audio_channel == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }

            break;
        case(INVADER_MOVE_3):
            audio_channel = Mix_PlayChannel(-1, audio_context->invader_move_3, 0);
            if (audio_channel == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }

            break;
        case(INVADER_MOVE_4):
            audio_channel = Mix_PlayChannel(-1, audio_context->invader_move_4, 0);
            if (audio_channel == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }

            break;
        case(PLAYER_KILLED):
            audio_channel = Mix_PlayChannel(-1, audio_context->player_killed, 0);
            if (audio_channel == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }

            break;
        case(PLAYER_SHOOT):
            audio_channel = Mix_PlayChannel(-1, audio_context->player_shoot, 0);
            if (audio_channel == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }

            break;
        case(UFO_KILLED):
            audio_channel = Mix_PlayChannel(-1, audio_context->ufo_killed, 0);
            if (audio_channel == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }

            break;
        case(UFO_MOVE):
            audio_channel = AudioSetUfoMovement(audio_context, ACTIVATE_UFO_MOVE);
            if (audio_channel == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }
            
            break;
        default:
            return AUDIO_FAILURE;
    }

    return AUDIO_SUCCESS;
}

void AudioShutdown(AudioContext *audio_context) {
    if (audio_context == NULL) {
        return;
    }


    if (audio_context->mixer_open) {
        Mix_HaltChannel(-1);

        if (audio_context->invader_killed != NULL) {
            Mix_FreeChunk(audio_context->invader_killed);
        }

        if (audio_context->invader_move_1 != NULL) {
            Mix_FreeChunk(audio_context->invader_move_1);
        }

        if (audio_context->invader_move_2 != NULL) {
            Mix_FreeChunk(audio_context->invader_move_2);
        }

        if (audio_context->invader_move_3 != NULL) {
            Mix_FreeChunk(audio_context->invader_move_3);
        }

        if (audio_context->invader_move_4 != NULL) {
            Mix_FreeChunk(audio_context->invader_move_4);
        }

        if (audio_context->player_killed != NULL) {
            Mix_FreeChunk(audio_context->player_killed);
        }

        if (audio_context->player_shoot != NULL) {
            Mix_FreeChunk(audio_context->player_shoot);
        }

        if (audio_context->ufo_killed != NULL) {
            Mix_FreeChunk(audio_context->ufo_killed);
        }

        if (audio_context->ufo_move != NULL) {
            Mix_FreeChunk(audio_context->ufo_move);
        }

        Mix_CloseAudio();
    }
    free(audio_context);

    return;
}
