/*WRITE Non-Canonical Input Processing*/
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include "llmacros.h"
#include "interface.h"


#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

appLayer app;
linkLayer llink;




int main(int argc, char** argv) {
  char header[STRSIZE] = {0x7E, 0x03, 0x03, 0x00,
                          0x7E};  // THIS IS THE CORRECT MESSAGE
  char field_data[35] = "amizade na faculdade de Engenharia";
  int c;
  unsigned char buf[STRSIZE];
  int i, sum = 0, speed = 0;

  if ((argc < 2) ||
      ((strcmp(COM_1, argv[1]) != 0) && (strcmp(COM_2, argv[1]) != 0) &&
       (strcmp(COM_3, argv[1]) != 0))) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttySx\n");
    exit(1);
  }

  llink.baudRate = BAUDRATE;
  llink.timeout = 2;
  llink.numTransmissions = 3;
  llopen(0,TRANSMITTER);
  llwrite(app.fileDescriptor,field_data,35);

  sleep(1);

  if (tcsetattr(app.fileDescriptor, TCSANOW, &llink.oldPortSettings) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  close(app.fileDescriptor);
  return 0;
}
