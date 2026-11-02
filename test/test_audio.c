#include <stdio.h>
#include <stdbool.h>

#include "audio.h"

int main(void) {
    AudioContext *audio_context = AudioInit();
    if (audio_context == NULL) {
        printf("AudioContext Initialization Failure\n");
        return 1;
    }

    AudioShutdown(audio_context);

    printf("Audio Test Sucessful\n");
    return 0;
}
