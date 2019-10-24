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
#include "interface.h"
#include "llmacros.h"
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
  char field_data[256];
  char field_data2[256];
  int c;
  unsigned char buf[STRSIZE];
  int i, sum = 0, speed = 0;

  if ((argc < 2) || (atoi(argv[1]) < 0 || atoi(argv[1]) > 2))
  {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttySx\n");
    exit(1);
  }

  char name[13] = "textfile.txt";
  FILE *file = fopen(name, "r");
  char *pointer = makeControlPacket('2', name, file);

  llink.baudRate = BAUDRATE;
  llink.timeout = 2;
  llink.numTransmissions = 3;
  llopen(atoi(argv[1]), RECEIVER);
  llread(app.fileDescriptor, field_data);
  llread(app.fileDescriptor, field_data2);
  llclose(app.fileDescriptor, RECEIVER);

  return 0;
}
