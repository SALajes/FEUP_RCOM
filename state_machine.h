#ifndef LLINK_STATEMACHINES
#define LLINK_STATEMACHINES
#include "llmacros.h"

void advance_state_SET(unsigned char byte, states *state);
void advance_state_UA(unsigned char byte, states *state);
void advance_state_UA_DISC(unsigned char byte, states *state);
void advance_state_I(unsigned char byte, states *state, int* disc);
void advance_state_RR(unsigned char byte, states *state);
void advance_state_DISC(unsigned char byte, states *state);

#endif