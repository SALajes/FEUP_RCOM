/* A_SND sent when the sender asks
*  A_RCV sent when the receiver answers 
*  
*
*/
#define FLAG_RCV 0x7E
#define A_SND 0x03
#define A_RCV 0x01
#define C_SND 0x03
#define C_RCV 0x07
#define BCC_SND A_SND ^ C_SND
#define BCC_RCV A_RCV ^ C_RCV

typedef enum {
    START = 0,
    FLAG_OK = 1,
    A_OK = 2,
    C_OK = 3,
    BCC_OK = 4,
    STOP = 5 
} states;

int advance_state_SET(int byte, states *state);
int advance_state_UA(int byte, states *state);
