#include "state_machine.h"

int advance_state_SET(char byte, states state) {
    if (state == START) {
        if (byte == FLAG_RCV) {
            state = FLAG_OK;
        }
    }
    else if (state == FLAG_OK) {
        if (byte == FLAG_RCV) {
        }
        else if (byte == A_RCV) {
            state = A_OK;
        }
        else {
            state = FLAG_OK;
        }
    }
    else if (byte == A_OK) {
        if (byte == C_RCV) {
            state = C_OK;
        }
        else if (byte == FLAG_RCV) {
            state = FLAG_OK;
        }
        else {
            state = START;
        }
    }
    else if (byte == C_OK) {
        if (byte == BCC_RCV) {
            state = BCC_OK;
        }
        else if (byte == FLAG_RCV) {
            state = FLAG_OK;
        }
        else {
            state = START;
        }
    }
    else if (byte == C_OK){}
    else if (byte == BCC_OK) {
        if (byte == FLAG_RCV) {
            state = STOP;
        }
        else {
            state = START;
        }
    }
    else if (byte == STOP) {
        
    }
}

int advance_state_UA(char byte, states state) {
    if (state == START) {
        if (byte == FLAG_RCV) {
            state = FLAG_OK;
        }
    }
    else if (state == FLAG_OK) {
        if (byte == FLAG_RCV) {
        }
        else if (byte == A_SND) {
            state = A_OK;
        }
        else {
            state = FLAG_OK;
        }
    }
    else if (byte == A_OK) {
        if (byte == C_SND) {
            state = C_OK;
        }
        else if (byte == FLAG_RCV) {
            state = FLAG_OK;
        }
        else {
            state = START;
        }
    }
    else if (byte == C_OK) {
        if (byte == BCC_SND) {
            state = BCC_OK;
        }
        else if (byte == FLAG_RCV) {
            state = FLAG_OK;
        }
        else {
            state = START;
        }
    }
    else if (byte == C_OK){}
    else if (byte == BCC_OK) {
        if (byte == FLAG_RCV) {
            state = STOP;
        }
        else {
            state = START;
        }
    }
    else if (byte == STOP) {
        
    }
}
