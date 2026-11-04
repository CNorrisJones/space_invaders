#include <stdio.h>
#include <stdbool.h>

#include "audio.h"

void wait_for_enter(void) {
    printf("Press ENTER to continue...");
    fflush(stdout); // Ensures the prompt displays immediately

    int c;
    // Loop consumes all characters in the stream up to and including the newline
    while ((c = getchar()) != '\n' && c != EOF); 
}

int main(void) {
    AudioContext *audio_context = AudioInit();
    if (audio_context == NULL) {
        printf("AudioContext Initialization Failure\n");
        return 1;
    }

    int ufo_audio = AudioSetUfoMovement(NULL, true);
    if (ufo_audio != AUDIO_FAILURE) {
        printf("AudioSetUfoMovement Did Not Fail Correctly with Null Audio Context and true Param\n");
        AudioShutdown(audio_context);
        return 1;
    }

    ufo_audio = AudioSetUfoMovement(NULL, false);
    if (ufo_audio != AUDIO_FAILURE) {
        printf("AudioSetUfoMovement Did Not Fail Correctly with Null Audio Context and false Param\n");
        AudioShutdown(audio_context);
        return 1;
    }

    ufo_audio = AudioSetUfoMovement(audio_context, true);
    if (ufo_audio != AUDIO_SUCCESS) {
        printf("AudioSetUfoMovement Did Not Set up Ufo Move Channel properly\n");
        AudioShutdown(audio_context);
        return 1;
    }

    ufo_audio = AudioSetUfoMovement(audio_context, true);
    if (ufo_audio != AUDIO_SUCCESS) {
        printf("AudioSetUfoMovement Did Not Set up Ufo Move Channel properly\n");
        AudioShutdown(audio_context);
        return 1;
    }

    ufo_audio = AudioSetUfoMovement(audio_context, false);
    if (ufo_audio != AUDIO_SUCCESS) {
        printf("AudioSetUfoMovement Did Not Set up Ufo Move Channel properly\n");
        AudioShutdown(audio_context);
        return 1;
    }

    ufo_audio = AudioSetUfoMovement(audio_context, false);
    if (ufo_audio != AUDIO_SUCCESS) {
        printf("AudioSetUfoMovement Did Not Set up Ufo Move Channel properly\n");
        AudioShutdown(audio_context);
        return 1;
    }

    int play_audio = PlayAudio(audio_context, INVADER_KILLED);
    if (play_audio != AUDIO_SUCCESS) {
        printf("PlayAudio failed to play effect 0\n");
        AudioShutdown(audio_context);
        return 1;
    }
    wait_for_enter();

    play_audio = PlayAudio(audio_context, INVADER_MOVE_1);
    if (play_audio != AUDIO_SUCCESS) {
        printf("PlayAudio failed to play effect 0\n");
        AudioShutdown(audio_context);
        return 1;
    }
    wait_for_enter();

    play_audio = PlayAudio(audio_context, INVADER_MOVE_2);
    if (play_audio != AUDIO_SUCCESS) {
        printf("PlayAudio failed to play effect 0\n");
        AudioShutdown(audio_context);
        return 1;
    }
    wait_for_enter();

    play_audio = PlayAudio(audio_context, INVADER_MOVE_3);
    if (play_audio != AUDIO_SUCCESS) {
        printf("PlayAudio failed to play effect 0\n");
        AudioShutdown(audio_context);
        return 1;
    }
    wait_for_enter();

    play_audio = PlayAudio(audio_context, INVADER_MOVE_4);
    if (play_audio != AUDIO_SUCCESS) {
        printf("PlayAudio failed to play effect 0\n");
        AudioShutdown(audio_context);
        return 1;
    }
    wait_for_enter();

    play_audio = PlayAudio(audio_context, PLAYER_KILLED);
    if (play_audio != AUDIO_SUCCESS) {
        printf("PlayAudio failed to play effect 0\n");
        AudioShutdown(audio_context);
        return 1;
    }
    wait_for_enter();

    play_audio = PlayAudio(audio_context, PLAYER_SHOOT);
    if (play_audio != AUDIO_SUCCESS) {
        printf("PlayAudio failed to play effect 0\n");
        AudioShutdown(audio_context);
        return 1;
    }
    wait_for_enter();

    play_audio = PlayAudio(audio_context, UFO_KILLED);
    if (play_audio != AUDIO_SUCCESS) {
        printf("PlayAudio failed to play effect 0\n");
        AudioShutdown(audio_context);
        return 1;
    }
    wait_for_enter();

     play_audio = PlayAudio(audio_context, UFO_MOVE);
    if (play_audio != AUDIO_SUCCESS) {
        printf("PlayAudio failed to play effect 0\n");
        AudioShutdown(audio_context);
        return 1;
    }
    wait_for_enter();

    AudioShutdown(audio_context);

    printf("Audio Test Sucessful\n");
    return 0;
}
