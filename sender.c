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
#include "state_machine.h"
#include "packet_factory.h"

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1


int counter = 0;
extern char packet[255];
int fd, res;
struct termios oldtio, newtio;

void write_info();

void alarm_handler() {
  tcflush(fd, TCIOFLUSH);

  if (counter == 3) {
    printf("Could not connect to receiver. Halting execution...\n");
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
      perror("tcsetattr");
      close(fd);
      exit(-1);
    }
    close(fd);
    exit(-1);
  }
  counter++;
  printf("Connection timed out. Retrying... (attempt number %d) \n", counter);
  write_info();
}

void write_info() {

  res = write(fd, packet, STRSIZE);

  printf("%d bytes written\n", res);

  alarm(3);
}

int main(int argc, char** argv) {
  char header[STRSIZE] = {0x7E, 0x03, 0x03, 0x00,0x7E};  // THIS IS THE CORRECT MESSAGE
  char field_data[256] = "amizade na faculdade de Engenharia";
  int c;
  unsigned char buf[STRSIZE];
  int i, sum = 0, speed = 0;

  signal(SIGALRM, alarm_handler);

  if ((argc < 2) || ((strcmp("/dev/ttyS0", argv[1]) != 0) &&
                     (strcmp("/dev/ttyS1", argv[1]) != 0) &&
                     (strcmp("/dev/ttyS2", argv[1]) != 0))) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
*/

  fd = open(argv[1], O_RDWR | O_NOCTTY);
  if (fd < 0) {
    perror(argv[1]);
    exit(-1);
  }

  if (tcgetattr(fd, &oldtio) == -1) { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
  newtio.c_cc[VMIN] = 1;  /* blocking read until 5 chars received */

  /*
  VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
  leitura do(s) pr�ximo(s) caracter(es)
*/

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");

  /*testing*/

  /*
  O ciclo FOR e as instru��es seguintes devem ser alterados de modo a respeitar
  o indicado no gui�o
*/

  makePacket(header,field_data);
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
