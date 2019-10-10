#include "state_machine.h"

void advance_state_SET(char byte, states* state) {
  switch (*state) {
    case START:
      if (byte == FLAG_RCV) *state = FLAG_OK;
      break;
    case FLAG_OK:
      if (byte == FLAG_RCV) *state = FLAG_OK;
      if (byte == A_RCV)    *state = A_OK;
      else *state = START;
      break;
    case A_OK:
      if (byte == C_RCV) *state = C_OK;
      if (byte == FLAG_RCV) *state = FLAG_OK;
      else *state = START;
      break;
    case C_OK:
      if (byte == BCC_RCV) *state = BCC_OK;
      if (byte == FLAG_RCV) *state = FLAG_OK;
      else *state = START;
      break;
    case BCC_OK:
      if (byte == FLAG_RCV) *state = STOP;
      else *state = START;
    default:
        break;
  }
}

void advance_state_UA(char byte, states* state) {
  switch (*state) {
    case START:
      if (byte == FLAG_RCV) *state = FLAG_OK;
      break;
    case FLAG_OK:
      if (byte == FLAG_RCV) *state = FLAG_OK;
      if (byte == A_SND)    *state = A_OK;
      else *state = START;
      break;
    case A_OK:
      if (byte == C_SND) *state = C_OK;
      if (byte == FLAG_RCV) *state = FLAG_OK;
      else *state = START;
      break;
    case C_OK:
      if (byte == BCC_SND) *state = BCC_OK;
      if (byte == FLAG_RCV) *state = FLAG_OK;
      else *state = START;
      break;
    case BCC_OK:
      if (byte == FLAG_RCV) *state = STOP;
      else *state = START;
    default:
        break;
  }
}
