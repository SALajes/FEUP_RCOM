#ifndef APPLAYER
#define APPLAYER

#define MAXFRAMESIZE 200
#include <stdio.h>
#define APP_C_DATA 1
#define APP_C_START 2
#define APP_C_END 3

typedef struct
{
    int fileDescriptor; /*Descritor correspondente à porta série*/
    int status;         /*TRANSMITTER | RECEIVER*/
    int lastchunk;      // index of the latest chunk read
} appLayer;

int applicationLayerSender(int port, char *file_name);
#endif
