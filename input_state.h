#ifndef INPUT_STATE_H
#define INPUT_STATE_H

#include <stdbool.h>

typedef struct InputState {
    bool credit;
    bool one_player;
    bool two_player;
    bool tilt;
    bool p1_left;
    bool p1_right;
    bool p1_fire;
    bool p2_left;
    bool p2_right;
    bool p2_fire;
} InputState;

#endif
