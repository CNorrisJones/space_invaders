#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL_mixer.h>

#include "audio.h"

static const int FREQUENCY = 44100;
static const int CHANNELS = 2;
static const int AUDIO_BUFFER_SIZE = 2048;

struct AudioContext {
    Mix_Chunk *invader_killed;
    Mix_Chunk *invader_move_1;
    Mix_Chunk *invader_move_2;
    Mix_Chunk *invader_move_3;
    Mix_Chunk *invader_move_4;
    Mix_Chunk *player_killed;
    Mix_Chunk *player_shoot;
    Mix_Chunk *ufo_killed;
    Mix_Chunk *ufo_move;
};

static int LoadWavIntoAudioContext(Mix_Chunk *audio_chunk, char filename[]) {
    audio_chunk = Mix_LoadWAV(filename);
    if(audio_chunk == NULL) {
        printf("Failed to load '%s' sound effect! SDL_mixer Error: %s\n", filename, Mix_GetError());

        return AUDIO_FAILURE;
    }
    
    return AUDIO_SUCCESS;
}

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

    //Initialize SDL_mixer
    int audio_init = Mix_OpenAudio(FREQUENCY, MIX_DEFAULT_FORMAT, CHANNELS, AUDIO_BUFFER_SIZE);
    if (audio_init == AUDIO_FAILURE) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        AudioShutdown(audio_context);
        return NULL;
    }

    int audio_loaded;
    audio_loaded = LoadWavIntoAudioContext(audio_context->invader_killed, "sound/invader_killed.wav");
    if(audio_loaded == AUDIO_FAILURE) {
        AudioShutdown(audio_context);
        return NULL;
    }

    audio_loaded = LoadWavIntoAudioContext(audio_context->invader_move_1, "sound/invader_move_1.wav");
    if(audio_loaded == AUDIO_FAILURE) {
        AudioShutdown(audio_context);
        return NULL;
    }

    audio_loaded = LoadWavIntoAudioContext(audio_context->invader_move_2, "sound/invader_move_2.wav");
    if(audio_loaded == AUDIO_FAILURE) {
        AudioShutdown(audio_context);
        return NULL;
    }

    audio_loaded = LoadWavIntoAudioContext(audio_context->invader_move_3, "sound/invader_move_3.wav");
    if(audio_loaded == AUDIO_FAILURE) {
        AudioShutdown(audio_context);
        return NULL;
    }

    audio_loaded = LoadWavIntoAudioContext(audio_context->invader_move_4, "sound/invader_move_4.wav");
    if(audio_loaded == AUDIO_FAILURE) {
        AudioShutdown(audio_context);
        return NULL;
    }

    audio_loaded = LoadWavIntoAudioContext(audio_context->player_killed, "sound/player_killed.wav");
    if(audio_loaded == AUDIO_FAILURE) {
        AudioShutdown(audio_context);
        return NULL;
    }

    audio_loaded = LoadWavIntoAudioContext(audio_context->player_shoot, "sound/player_shoot.wav");
    if(audio_loaded == AUDIO_FAILURE) {
        AudioShutdown(audio_context);
        return NULL;
    }

    audio_loaded = LoadWavIntoAudioContext(audio_context->ufo_killed, "sound/ufo_killed.wav");
    if(audio_loaded == AUDIO_FAILURE) {
        AudioShutdown(audio_context);
        return NULL;
    }

    audio_loaded = LoadWavIntoAudioContext(audio_context->ufo_move, "sound/ufo_move.wav");
    if(audio_loaded == AUDIO_FAILURE) {
        AudioShutdown(audio_context);
        return NULL;
    }

    return audio_context;
}

int PlayAudio(AudioContext *audio_context, int audio_flag) {
    if (audio_context == NULL) {
        return AUDIO_FAILURE;
    }

    int audio_play;

    switch(audio_flag) {
        case(INVADER_KILLED):
            audio_play = Mix_PlayChannel(-1, audio_context->invader_killed, 0);
            if (audio_play == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }

            break;
        case(INVADER_MOVE_1):
            audio_play = Mix_PlayChannel(-1, audio_context->invader_move_1, 0);
            if (audio_play == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }

            break;
        case(INVADER_MOVE_2):
            audio_play = Mix_PlayChannel(-1, audio_context->invader_move_2, 0);
            if (audio_play == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }

            break;
        case(INVADER_MOVE_3):
            audio_play = Mix_PlayChannel(-1, audio_context->invader_move_3, 0);
            if (audio_play == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }

            break;
        case(INVADER_MOVE_4):
            audio_play = Mix_PlayChannel(-1, audio_context->invader_move_4, 0);
            if (audio_play == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }

            break;
        case(PLAYER_KILLED):
            audio_play = Mix_PlayChannel(-1, audio_context->player_killed, 0);
            if (audio_play == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }

            break;
        case(PLAYER_SHOOT):
            audio_play = Mix_PlayChannel(-1, audio_context->player_shoot, 0);
            if (audio_play == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }

            break;
        case(UFO_KILLED):
            audio_play = Mix_PlayChannel(-1, audio_context->ufo_killed, 0);
            if (audio_play == AUDIO_FAILURE) {
                return AUDIO_FAILURE;
            }

            break;
        case(UFO_MOVE):
            audio_play = Mix_PlayChannel(-1, audio_context->ufo_move, 0);
            if (audio_play == AUDIO_FAILURE) {
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
    free(audio_context);

    return;
}
