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
#include "applicationLayer.h"

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

extern appLayer app;
extern linkLayer llink;

int main(int argc, char **argv)
{
  char field_data[35] = "amizade na faculdade de Engenharia";
  char field_data2[18] = "ola sou a vanessa";
  int c;
  unsigned char buf[STRSIZE];
  int i, sum = 0, speed = 0;

  if ((argc < 2) || (atoi(argv[1]) < 0 || atoi(argv[1]) > 2))
  {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttySx\n");
    exit(1);
  }

  llink.baudRate = BAUDRATE;
  llink.timeout = 2;
  llink.numTransmissions = 3;

  llopen(atoi(argv[1]), TRANSMITTER);
  llwrite(app.fileDescriptor, field_data, 35);
  llwrite(app.fileDescriptor, field_data2, 18);
  llclose(app.fileDescriptor, TRANSMITTER);

  sleep(1);

  return 0;
}
