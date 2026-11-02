#ifndef AUDIO_H
#define AUDIO_H

enum AudioStatus {
    AUDIO_FAILURE = -1,
    AUDIO_SUCCESS = 0
};

enum AudioFlags {
    INVADER_KILLED  = 0,
    INVADER_MOVE_1  = 1,
    INVADER_MOVE_2  = 2,
    INVADER_MOVE_3  = 3,
    INVADER_MOVE_4  = 4,
    PLAYER_KILLED   = 5,
    PLAYER_SHOOT    = 6,
    UFO_KILLED      = 7,
    UFO_MOVE        = 8,
};

typedef struct AudioContext
AudioContext;

AudioContext *AudioInit(void);
int PlayAudio(AudioContext *audio_context, int audio_flag);
void AudioShutdown(AudioContext *audio_context);

#endif
