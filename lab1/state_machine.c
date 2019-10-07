#include "state_machine.h"

int advance_state_SET(int byte, states *state) {
    if (*state == START) {
        if (byte == FLAG_RCV) {
            *state = FLAG_OK;
        }
    }
    else if (*state == FLAG_OK) {
        if (byte == FLAG_RCV) {} //mantem se
        else if (byte == A_RCV) {
            *state = A_OK;
        }
        else {
            *state = START;
        }
    }
    else if (*state == A_OK) {
        if (byte == C_RCV) {
            *state = C_OK;
        }
        else if (byte == FLAG_RCV) {
            *state = FLAG_OK;
        }
        else {
            *state = START;
        }
    }
    else if (*state == C_OK) {
        if (byte == BCC_RCV) {
            *state = BCC_OK;
        }
        else if (byte == FLAG_RCV) {
            *state = FLAG_OK;
        }
        else {
            *state = START;
        }
    }
    else if (*state  == BCC_OK) {
        if (byte == FLAG_RCV) {
            *state = STOP;
        }
        else {
            *state = START;
        }
    }
    else if (*state == STOP) {}
}

int advance_state_UA(int byte, states *state) {
    if (*state == START) {
        if (byte == FLAG_RCV) {
            *state = FLAG_OK;
        }
    }
    else if (*state == FLAG_OK) {
        if (byte == FLAG_RCV) {}
        else if (byte == A_SND) {
            *state = A_OK;
        }
        else {
            *state = START;
        }
    }
    else if (*state == A_OK) {
        if (byte == C_SND) {
            *state = C_OK;
        }
        else if (byte == FLAG_RCV) {
            *state = FLAG_OK;
        }
        else {
            *state = START;
        }
    }
    else if (*state == C_OK) {
        if (byte == BCC_SND) {
            *state = BCC_OK;
        }
        else if (byte == FLAG_RCV) {
            *state = FLAG_OK;
        }
        else {
            *state = START;
        }
    }
    else if (*state == BCC_OK) {
        if (byte == FLAG_RCV) {
            *state = STOP;
        }
        else {
            *state = START;
        }
    }
    else if (*state == STOP) {}
}
