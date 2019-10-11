#ifndef LLMACROS
#define LLMACROS
#include <termios.h>

#define COM_1 "/dev/ttyS0"
#define COM_2 "/dev/ttyS2"
#define COM_3 "/dev/ttyS3"

#define STRSIZE 5
#define FLAG_RCV 0x7E
#define A_SND 0x03
#define A_RCV 0x01
#define C_SND 0x03
#define C_RCV 0x07
#define BCC_SND (A_SND ^ C_SND)
#define BCC_RCV (A_RCV ^ C_RCV)

#define ESC_OCT 0x7D
#define ESC_STF ESC_OCT ^ 0x20
#define FLAG_OCT 0x7E
#define FLAG_STF FLAG_RCV ^ 0x20


#define TRANSMITTER 0
#define RECEIVER 1

typedef enum
{
    START = 0,
    FLAG_OK = 1,
    A_OK = 2,
    C_OK = 3,
    BCC_OK = 4,
    STOP = 5
} states;

typedef struct  {
  int fileDescriptor; /*Descritor correspondente à porta série*/
  int status;         /*TRANSMITTER | RECEIVER*/
} appLayer;

typedef struct  {
  char port[20];                  /*Dispositivo /dev/ttySx, x = 0, 1*/
  int baudRate;                   /*Velocidade de transmissão*/
  struct termios oldPortSettings; /*Settings antigas da Porta*/
  struct termios newPortSettings; /*Settings novas da Porta*/                
  unsigned int sequenceNumber;    /*Número de sequência da trama: 0, 1*/
  unsigned int timeout;           /*Valor do temporizador: 1 s*/
  unsigned int numTransmissions;  /*Número de tentativas em caso de falha*/
  char frame[255];                /*Trama*/
  unsigned int frame_size;
} linkLayer;

#endif