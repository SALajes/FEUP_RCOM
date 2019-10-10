/*WRITE Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "state_machine.h"
#include <unistd.h>
#include <signal.h>

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define STRSIZE 5

#define FLAG_RCV 0x7E
#define A_RCV1 0x03
#define A_RCV2 0x01
#define C_RCV1 0x03
#define C_RCV2 0x07

int counter = 0;
unsigned char str[STRSIZE] = {0x7E, 0x03, 0x03, 0x00, 0x7E}; //THIS IS THE CORRECT MESSAGE
int fd, res;
struct termios oldtio, newtio;

void alarm_handler()
{
  tcflush(fd, TCIOFLUSH);

  if (counter == 3)
  {
    printf("Could not connect to receiver. Halting execution...\n");
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
    {
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

void write_info()
{
  res = write(fd, str, STRSIZE);

  printf("%d bytes written\n", res);

  alarm(3);
}

char *stuff(char *array)
{
  char *aux_array = malloc(STRSIZE * 2);
  unsigned int i, j;
  for (i = 0, j = 0; i < STRSIZE; i++, j++)
  {
    if (array[i] == 0x7E)
    {
      aux_array[j] = 0x7D;
      aux_array[j + 1] = 0x5E;
      j++;
    }
    else
    {
      aux_array[j] = array[i];
    }
  }
  return aux_array;
}

int main(int argc, char **argv)
{
  int c;
  unsigned char buf[STRSIZE];
  int i, sum = 0, speed = 0;

  signal(SIGALRM, alarm_handler);

  if ((argc < 2) ||
      ((strcmp("/dev/ttyS0", argv[1]) != 0) &&
       (strcmp("/dev/ttyS1", argv[1]) != 0) &&
       (strcmp("/dev/ttyS2", argv[1]) != 0)))
  {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
*/

  fd = open(argv[1], O_RDWR | O_NOCTTY);
  if (fd < 0)
  {
    perror(argv[1]);
    exit(-1);
  }

  if (tcgetattr(fd, &oldtio) == -1)
  { /* save current port settings */
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

  if (tcsetattr(fd, TCSANOW, &newtio) == -1)
  {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");

  /*testing*/

  /* 
  O ciclo FOR e as instru��es seguintes devem ser alterados de modo a respeitar 
  o indicado no gui�o 
*/

  write_info();

  states state_machine = START;

  for (int i = 0; state_machine != STOP; i++)
  {
    res = read(fd, &buf[i], 1);
    printf("BYTE: %#x\n", buf[i]);
    advance_state_SET(buf[i], &state_machine);
    printf("STATE: %d\n", state_machine);
  }

  stuff(str);

  sleep(1);

  if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
  {
    perror("tcsetattr");
    exit(-1);
  }

  close(fd);
  return 0;
}