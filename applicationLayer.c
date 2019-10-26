#include "applicationLayer.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "interface.h"
#include "llmacros.h"

#define BAUDRATE B9600
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

appLayer app;
linkLayer llink;

unsigned char* control_packet;

void processControlpacket(unsigned char* packet);

unsigned int makeControlPacket(char* file_name, FILE* file, int control_byte) {
  // tamanho do ficheiro
  unsigned int size = 0;
  char useless_buffer[256];

  while (!feof(file)) {
    size += fread(useless_buffer, sizeof(char), 256, file);
  }

  printf("%lu\n", size);
  unsigned char T1 = 0;
  unsigned char L1 = 4;
  unsigned char aux_array[8];  // um long tem NO MAXIMO 8 bytes ocupados

  int index = 0;

  // while (size > 0) {
  //   unsigned char byte =
  //       (unsigned char)size;  // recolho no byte os ultimos 8 bits

  //   // coloco num array PELA ORDEM CONTRARIA conforme recolho os bits
  //   aux_array[index] = byte;

  //   size = size >>
  //          8;  // faço o shift right para de seguida recolher os proximos 8 bits

  //   L1++;
  //   index++;
  // }

  unsigned char* V1 = (unsigned char *)&size;

  rewind(file);

  // nome do ficheiro
  unsigned char T2 = 1;
  char* V2 = file_name;
  unsigned char L2 = strlen(V2);

  control_packet = malloc(5 + L1 + L2);
  unsigned char* it = control_packet;

  // control_packet[0] is reserved for the control byte!!!
  *it = (unsigned char)control_byte;
  printf("Control byte\n");
  it++;
  *it = T1;
  printf("T1\n");
  it++;
  *it = L1;
  printf("L1\n");
  it++;
  memcpy(it, V1, L1);
  printf("V1\n");
  it += L1;
  *it = T2;
  printf("T2\n");
  it++;
  *it = L2 + 1;  // plus one for the \0 byte at the end of the string
  printf("L2\n");
  it++;
  memcpy(it, V2, L2);
  printf("V2\n");

  return L2 + L1 + 5;
}

// application layer sender
int applicationLayerSender(int port, char* file_name) {
  int controlp_size = 0;
  FILE* file = fopen(file_name, "r");

  if (file == NULL) {
    perror("Failed to open file\n");
    return -1;
  }

  llink.baudRate = BAUDRATE;
  llink.timeout = 2;
  llink.numTransmissions = 3;

  llopen(port, TRANSMITTER);
  // Make and Write Control Packet
  controlp_size = makeControlPacket(file_name, file, APP_C_START);
  printf("mandei%d\n", controlp_size);
  for (size_t i = 0; i < controlp_size; i++) {
    printf("%#x\n", control_packet[i]);
  }

  llwrite(app.fileDescriptor, (char*)control_packet, controlp_size);

  // this cycle: is meant to first send a control packet using llwrite to start
  // the transmition! control = 2 -> writeControlPacket(char control) is meant
  // for writing UNTIL it reaches end of file is meant to, at last, send a
  // control packet using llwrite to end the transmition! control = 3 ->
  // writeControlPacket(char control)

  // controlp_size = makeControlPacket(file_name, file, APP_C_END);
  // llwrite(app.fileDescriptor, control_packet, controlp_size);
  printf("rip oh filho\n");
  return llclose(app.fileDescriptor, TRANSMITTER);
}

// application layer receiver
int applicationLayerReceiver(int port) {
  int packet_size = 0;
  llink.baudRate = BAUDRATE;
  llink.timeout = 2;
  llink.numTransmissions = 3;
  unsigned char packet[MAX_DATA_PACKET_SIZE];

  llopen(port, RECEIVER);

  printf("Start\n");

  // while (1) {
  packet_size = llread(app.fileDescriptor, packet);
  for (size_t i = 0; i < packet_size; i++) {
    printf("%#x\n", packet[i]);
  }
  switch (packet[0]) {
    case APP_C_DATA:
      break;
    case APP_C_START:
      processControlpacket(packet);
      break;
    default:
      break;
  }

  if (packet[0] == APP_C_END) {
    perror("Wrong control byte for app layer packet.");
    // break;
  }
  // }

  // packet_size = llread(app.fileDescriptor, packet);

  // printf("End\n");

  // for (size_t i = 0; i < packet_size; i++)
  // {
  //     printf("%#x\n", packet[i]);
  // }

  /*
  this cycle : is meant for reading UNTIL: start control frame is received, it
  reaches end of file and finally receives end control frame no receiver o nome
  do file é recebido através da primeira trama I, recebendo no packet de dados
  dessa o tamanho e o nome do ficheiro e abrindo um file com
  mesmonome_received.extensão
  */

  return llclose(app.fileDescriptor, RECEIVER);
}

void processControlpacket(unsigned char* packet) {
  char file_name[MAX_FRAME_SIZE];
  int checks[2] = {0,0}; // 0 for app t file size, 1 for app t file name
  unsigned char* it = packet + 1;  // skipped Control byte
  while (1) {
    if (*it == APP_T_FILE_SIZE) {
      it++;  // now points to Length
      int length = *it;
      it++;  // now points to Byte size
      int value = 0;
      char* value_arr = (char*)&value;
      for (size_t i = 0; i < length; it++, i++) {
        unsigned char byte = *it;
        value_arr[i] = byte;
      }
      // it points to next T byte
      app.file_size = value;
      printf("File_size: %d\n", app.file_size);
      checks[0] = 1;
    }
    if (*it == APP_T_FILE_NAME) {
      it++;  // now points to Length
      int length = *it;
      it++;  // now points to First character
      char value_arr[length-1];
      for (size_t i = 0; i < length; it++, i++) {
        value_arr[i] = *it;
      }
      memcpy(&app.file_name, value_arr, length);
      printf("File name = %s\n", app.file_name);
      checks[1] = 1;
    }
    if (checks[1] && checks[0]){
      break;
    }
  }
}
