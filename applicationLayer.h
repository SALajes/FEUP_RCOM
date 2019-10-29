#ifndef APPLAYER
#define APPLAYER

#include <stdio.h>
#include "llmacros.h"
#define APP_C_DATA 1
#define APP_C_START 2
#define APP_C_END 3

#define APP_T_FILE_SIZE 0
#define APP_T_FILE_NAME 1

typedef struct
{
    int fileDescriptor; /*Descritor correspondente à porta série*/
    int status;         /*TRANSMITTER | RECEIVER*/
    int lastchunk;      // index of the latest chunk read
    char file_name[MAX_DATA_PACKET_SIZE];
    FILE* file;
    char packet[MAX_DATA_PACKET_SIZE];
    int file_size;
} appLayer;


int applicationLayerSender(int port, char *file_name);
int applicationLayerReceiver(int port);
#endif
