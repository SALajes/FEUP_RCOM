#include "set_stmachine.h"

typedef enum {
    START,
    FLAG_OK,
    A_OK,
    C_OK,
    BCC_OK,
    STOP 
} states ;


int state_machine(){
    states current_state = START;
    switch (current_state)
    {
    case START:
        /* code */
        break;
    case FLAG_OK:
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