#define MAXFRAMESIZE 200
#include <stdio.h>
// #define START 2
// #define END 3

typedef struct
{
    int fileDescriptor; /*Descritor correspondente à porta série*/
    int status;         /*TRANSMITTER | RECEIVER*/
} appLayer;

char *makeControlPacket(char control, char *file_name, FILE *file);