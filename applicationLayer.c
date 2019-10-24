#include "applicationLayer.h"
#include "llmacros.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BAUDRATE B9600
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

appLayer app;
linkLayer llink;

char *control_packet;

void makeControlPacket(char *file_name, FILE *file)
{
    //tamanho do ficheiro
    unsigned long size = 0;
    char *useless_buffer[256];

    while(!feof(file)){
        size += fread(useless_buffer, sizeof(char), 256, file);
    }

    unsigned char T1 = 0;
    unsigned char L1 = 0;
    unsigned char aux_array[8]; //um long tem NO MAXIMO 8 bytes ocupados

    int index = 0;

    while(size > 0){
        unsigned char byte = (unsigned char) size;  //recolho no byte os ultimos 8 bits 

        //coloco num array PELA ORDEM CONTRARIA conforme recolho os bits 
        aux_array[index] = byte;

        size = size >> 8; //faço o shift right para de seguida recolher os proximos 8 bits

        L1++;
        index++;
    }

    unsigned char V1[L1];

    //como esta escrito pela ordem contraria no aux array, vamos inverter agora a ordem ao guardar em V1 
    for(int i = L1 - 1, j = 0; i > 0; i--, j++){
        V1[j] = aux_array[i];
    }

    rewind(file);

    //nome do ficheiro
    unsigned char T2 = 1;
    unsigned char L2;
    unsigned char *V2 = file_name;

    *control_packet = malloc(1 * 5 + sizeof(V1) + sizeof(V2));

    //control_packet[0] is reserved for the control byte!!!
    memcpy(control_packet + 1, T1, 1);
    memcpy(control_packet + 2, L1, 1);
    memcpy(control_packet + 3, V1, sizeof(V1));
    memcpy(control_packet + sizeof(V1), T2, 1);
    memcpy(control_packet + 8, L2, 1);
    memcpy(control_packet + 9, V2, sizeof(V2));
}

void writeControlPacket(char control){
    control_packet[0] = control;

    llwrite(app.fileDescriptor, control_packet, sizeof(control_packet));
}

//application layer sender
int applicationLayerSender(int port, char* file_name){
    FILE *file = fopen(file_name, "r");

    if(file == NULL){
        perror("Failed to open file\n");
        return -1;
    }

    makeControlPacket(file_name, file);
    
    llink.baudRate = BAUDRATE;
    llink.timeout = 2;
    llink.numTransmissions = 3;

    llopen(port, TRANSMITTER);   

    // this cycle: is meant to first send a control packet using llwrite to start the transmition! control = 2 -> writeControlPacket(char control)
    // this cycle : is meant for writing UNTIL it reaches end of file
    // this cycle : is meant to, at last, send a control packet using llwrite to end the transmition! control = 3 -> writeControlPacket(char control)

    return llclose(app.fileDescriptor, TRANSMITTER);
}

//application layer receiver
int applicationLayerReceiver(int port){
    llink.baudRate = BAUDRATE;
    llink.timeout = 2;
    llink.numTransmissions = 3;

    llopen(port, RECEIVER);
    
    /*
    this cycle : is meant for reading UNTIL: start control frame is received, it reaches end of file and finally receives end control frame
    no receiver o nome do file é recebido através da primeira trama I, recebendo no packet de dados dessa o tamanho e o nome do ficheiro e abrindo um file com mesmonome_received.extensão
    */
    
    return llclose(app.fileDescriptor, RECEIVER);
}