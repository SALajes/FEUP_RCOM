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
#include "packet_factory.h"
#include "state_machine.h"


#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

appLayer app;
linkLayer llink;

int counter = 0;
void (*alarm_func)();
extern char packet[255];
int fd, res;
struct termios oldtio, newtio;




void send_data_packet() {
  res = write(fd, packet, STRSIZE);

  printf("%d bytes written\n", res);

  alarm(3);
}

int main(int argc, char** argv) {
  char header[STRSIZE] = {0x7E, 0x03, 0x03, 0x00,
                          0x7E};  // THIS IS THE CORRECT MESSAGE
  char field_data[256] = "amizade na faculdade de Engenharia";
  int c;
  unsigned char buf[STRSIZE];
  int i, sum = 0, speed = 0;

  signal(SIGALRM, alarm_handler);

  if ((argc < 2) ||
      ((strcmp(COM_1, argv[1]) != 0) && (strcmp(COM_2, argv[1]) != 0) &&
       (strcmp(COM_3, argv[1]) != 0))) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttySx\n");
    exit(1);
  }

  makePacket(header, field_data);
  write_info();

  states state_machine = START;

  for (int i = 0; state_machine != STOP; i++) {
    res = read(fd, &buf[i], 1);
    printf("BYTE: %#x\n", buf[i]);
    advance_state_SET(buf[i], &state_machine);
    printf("STATE: %d\n", state_machine);
  }

  sleep(1);

  if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  close(fd);
  return 0;
}
