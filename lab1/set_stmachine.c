#include "set_stmachine.h"

int advance_state(char byte, states state) {
    states current_state = START;
    switch (current_state) 
    {
        case START:
            /* code */
            break;
        case FLAG_OK:
            if (byte == 
            /* code */
            break;
        case A_OK:
            /* code */
            break;
        case C_OK:
            /* code */
            break;
        case BCC_OK:
            /* code */
            break;
        case STOP:
            /* code */
            break;
        default:
            current_state = START;
            break;
    }
}
