#ifndef LLMACROS
#define LLMACROS
#include <termios.h>

#define COM_1 "/dev/ttyS0"
#define COM_2 "/dev/ttyS2"
#define COM_3 "/dev/ttyS3"

#define STRSIZE 4
#define FLAG 0x7E
#define A_SND    0x03
#define A_RCV    0x01
#define C_SET    0x03
#define C_DISC   0x0B
#define C_UA     0x07
#define C_RR0    0x05
#define C_RR1    0x85
#define C_REJ0   0x01
#define C_REJ1   0x81
#define C_I0     0x00
#define C_I1     0x40

#define ESC_OCT 0x7D
#define ESC_STF ESC_OCT ^ 0x20
#define FLAG_OCT 0x7E
#define FLAG_STF FLAG ^ 0x20

#define TRANSMITTER 0
#define RECEIVER 1

#define MAX_FRAME_SIZE 255
#define MAX_DATA_PACKET_SIZE (MAX_FRAME_SIZE - 6) 

typedef enum
{
    START = 0,
    FLAG_OK ,
    A_OK ,
    C_OK ,
    BCC_OK,
    DATA_R,
    STOP 
} states;

typedef struct  {
  char port[20];                  /*Dispositivo /dev/ttySx, x = 0, 1*/
  int baudRate;                   /*Velocidade de transmissão*/
  struct termios oldPortSettings; /*Settings antigas da Porta*/
  struct termios newPortSettings; /*Settings novas da Porta*/                
  unsigned int sequenceNumber;    /*Número de sequência da trama: 0, 1*/
  unsigned int timeout;           /*Valor do temporizador: 1 s*/
  unsigned int numTransmissions;  /*Número de tentativas em caso de falha*/
  char frame[MAX_FRAME_SIZE];                /*Trama*/
  unsigned int frame_size;
} linkLayer;

typedef enum{
  SET,
  DISC,
  UA,
  RR,
  REJ, 
  CPRT
} control_t;

#endif