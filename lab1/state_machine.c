#include "state_machine.h"

int advance_state_SET(char byte, states state) {
    switch (current_state) 
    {
        case START:
            if (byte == FLAG_RCV) {
                state = FLAG_OK;
            }
            /* code */
            break;
        case FLAG_OK:
            if (byte == FLAG_RCV) {
            }
            else if (byte == A_RCV) {
                state = A_OK;
            }
            else {
                state = FLAG_OK;
            }
            break;
        case A_OK:
            if (byte == C_RCV) {
                state = C_OK;
            }
            else if (byte == FLAG_RCV) {
                state = FLAG_OK;
            }
            else {
                state = START;
            }
            break;
        case C_OK:
            if (byte == BCC_RCV) {
                state = BCC_OK;
            }
            else if (byte == FLAG_RCV) {
                state = FLAG_OK;
            }
            else {
                state = START;
            }
            break;
        case BCC_OK:
            if (byte == FLAG_RCV) {
                state = STOP;
            }
            else {
                state = START;
            }
            break;
        case STOP:
            /* code */
            break;
        default:
            current_state = START;
            break;
    }
}

int advance_state_UA(char byte, states state) {
    switch (current_state) 
    {
        case START:
            if (byte == FLAG_SND) {
                state = FLAG_OK;
            }
            /* code */
            break;
        case FLAG_OK:
            if (byte == FLAG_SND) {
            }
            else if (byte == A_SND) {
                state = A_OK;
            }
            else {
                state = FLAG_OK;
            }
            break;
        case A_OK:
            if (byte == C_SND) {
                state = C_OK;
            }
            else if (byte == FLAG_SND) {
                state = FLAG_OK;
            }
            else {
                state = START;
            }
            break;
        case C_OK:
            if (byte == BCC_SND) {
                state = BCC_OK;
            }
            else if (byte == FLAG_SND) {
                state = FLAG_OK;
            }
            else {
                state = START;
            }
            break;
        case BCC_OK:
            if (byte == FLAG_SND) {
                state = STOP;
            }
            else {
                state = START;
            }
            break;
        case STOP:
            /* code */
            break;
        default:
            current_state = START;
            break;
    }
}
