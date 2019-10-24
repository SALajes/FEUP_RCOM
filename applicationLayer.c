#include "applicationLayer.h"
#include "llmacros.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char field_data[35] = "amizade na faculdade de Engenharia";
char field_data2[18] = "ola sou a vanessa";
int c;
unsigned char buf[STRSIZE];
int i, sum = 0, speed = 0;

char *makeControlPacket(char control, char *file_name, FILE *file)
{
    // //tamanho do ficheiro
    // fseek(file, 0, SEEK_END);
    // int size = ftell(file);
    // rewind(file);

    // unsigned char T1 = 0;
    // unsigned char L1 = 4;
    // unsigned char V1[4];
    // sprintf(V1, "%d", size);

    // //nome do ficheiro
    // unsigned char T2 = 1;
    // unsigned char L2;
    // unsigned char *V2 = file_name;

    // char *control_packet = malloc(1 * 5 + sizeof(V1) + sizeof(V2));

    // memcpy(control_packet, T1, 1);
    // memcpy(control_packet + 1, L1, 1);
    // memcpy(control_packet + 2, V1, 4);
    // memcpy(control_packet + 6, T2, 1);
    // memcpy(control_packet + 7, L2, 1);
    // memcpy(control_packet + 8, V2, sizeof(file_name));
}

//application layer sender

//application layer receiver

//make control packet (CONTROLO , FILE_NAME)

//make data packet (CONTROLO = 1, FILE_NAME)