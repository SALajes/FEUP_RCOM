#include "applicationLayer.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "interface.h"
#include "llmacros.h"

appLayer app;
linkLayer llink;


void processControlpacket(unsigned char* packet);
void processDatapacket(unsigned char* packet, int file);

unsigned int makeDataPacket(FILE* file, int packet_size) {
  const int header_size = 4;
  unsigned char data[MAX_DATA_PACKET_SIZE];
  unsigned char* it = data;
  int size = fread(data + header_size, sizeof(unsigned char),
                   (!packet_size) ? MAX_FRAME_SIZE/2 - 4 : packet_size, file); // if packet size is 0, use the max size for packet
  *it = APP_C_DATA;
  it++;
  *it = (unsigned char)(app.lastchunk);
  it++;
  int L1 = size / 256;
  int L2 = size % 256;
  memcpy(it, (unsigned char*)&L1, 1);
  it++;
  memcpy(it, (unsigned char*)&L2, 1);
  it++;
  memcpy(app.packet, data, size + header_size);
  return size + header_size;
}

unsigned int makeControlPacket(char* file_name, FILE* file, int control_byte) {
  // tamanho do ficheiro
  unsigned char* control_packet;
  unsigned int size = 0;
  char useless_buffer[1];

  while (!feof(file)) {
    size += fread(useless_buffer, sizeof(unsigned char), 1, file);
  }

  unsigned char T1 = 0;
  unsigned char L1 = 4;
  unsigned char aux_array[8];  // um long tem NO MAXIMO 8 bytes ocupados

  int index = 0;
  unsigned char* V1 = (unsigned char*)&size;

  rewind(file);

  // nome do ficheiro
  unsigned char T2 = 1;
  char* V2 = file_name;
  unsigned char L2 = strlen(V2);

  control_packet = malloc(5 + L1 + L2);
  unsigned char* it = control_packet;

  // control_packet[0] is reserved for the control byte!!!
  *it = (unsigned char)control_byte;
  it++;
  *it = T1;
  it++;
  *it = L1;
  it++;
  memcpy(it, V1, L1);
  it += L1;
  *it = T2;
  it++;
  *it = L2 + 1;  // plus one for the \0 byte at the end of the string
  it++;
  memcpy(it, V2, L2);
  memcpy(app.packet, control_packet, L2 + L1 + 5);
  free(control_packet);
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

  app.lastchunk = 0;
  llink.baudRate = BAUDRATE;
  llink.timeout = 2;
  llink.numTransmissions = 3;

  llopen(port, TRANSMITTER);
  // Make and Write Control Packet
  controlp_size = makeControlPacket(file_name, file, APP_C_START);

  llwrite(app.fileDescriptor, (unsigned char*)app.packet, controlp_size);
	
	printf("Started file transmission\n");

  while (!feof(file)) {
    app.packet[0] = 0;
    controlp_size = makeDataPacket(file,0);
    if(controlp_size == 0){
      break;
    }

	printf("Mandei packet %d\n", app.lastchunk);

    llwrite(app.fileDescriptor, (unsigned char*)app.packet, controlp_size);
    app.lastchunk++;
    app.lastchunk %= 255;
  }

  controlp_size = makeControlPacket(file_name, file, APP_C_END);

	printf("Ended file transmission\n");

  llwrite(app.fileDescriptor, app.packet, controlp_size);

  return llclose(app.fileDescriptor, TRANSMITTER);
}

// application layer receiver
int applicationLayerReceiver(int port) {
  FILE* file;
  int file_fd;
  int packet_size = 0;
  app.lastchunk = 255;
  llink.baudRate = BAUDRATE;
  llink.timeout = 2;
  llink.numTransmissions = 3;
  unsigned char* packet = malloc(MAX_DATA_PACKET_SIZE*sizeof(unsigned char));

  llopen(port, RECEIVER);

  while (1) {
    packet_size = llread(app.fileDescriptor, packet);
    switch (packet[0]) {
      case APP_C_DATA:
        processDatapacket(packet, file_fd);
		printf("li o %d\n", app.lastchunk);        
		continue;
      case APP_C_START:
        processControlpacket(packet);
        file = fopen(app.file_name, "w+");
        //file = fopen("result.gif", "w+");
        file_fd = fileno(file);
        app.file = file;
        continue;
      default:
        break;
    }

    if (packet[0] == APP_C_END) {
      break;
    }
  }
  free(packet);
  fclose(file);
  return llclose(app.fileDescriptor, RECEIVER);
}

void processControlpacket(unsigned char* packet) {
  char file_name[MAX_FRAME_SIZE];
  int checks[2] = {0, 0};  // 0 for app t file size, 1 for app t file name
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
      checks[0] = 1;
    }
    if (*it == APP_T_FILE_NAME) {
      it++;  // now points to Length
      int length = *it;
      it++;  // now points to First character
      char value_arr[length - 1];
      for (size_t i = 0; i < length; it++, i++) {
        value_arr[i] = *it;
      }
      memcpy(&app.file_name, value_arr, length);
      checks[1] = 1;
    }
    if (checks[1] && checks[0]) {
      break;
    }
  }
}

void processDatapacket(unsigned char* packet, int file) {
  if (packet == NULL) {
    perror("Segmentation fault in processDataPacket");
    exit(6);
  }
  unsigned char* it = packet + 1;  // skip control byte
  // Read Sequence
  unsigned char sequence_number = *it;  // sequence number of last packet
  // if (sequence_number != (app.lastchunk + 1) % 255) {
  //   perror("Package sequence is wrong, file may be corrupted, exiting...
  //   \n"); exit(5);
  // }
  app.lastchunk = (int)sequence_number;
  it++;
  // Read data size = L1*256 + L2
  int L1 = (int)(*it);
  it++;
  int L2 = (int)(*it);
  it++;
  int data_size = L1 * 256 + L2;

  // Write data on file
  write(file, it, data_size * sizeof(unsigned char));
}
