/* A_SND sent when the sender asks
*  A_RCV sent when the receiver answers 
*  
*
*/
#include "llmacros.h"

void advance_state_SET(char byte, states *state);
void advance_state_UA(char byte, states *state);
void advance_state_I(char byte, states *state);
void advance_state_RR(unsigned char byte, states *state);
void advance_state_DISC(unsigned char byte, states *state);
