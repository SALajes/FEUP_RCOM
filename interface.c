#include "interface.h"
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

extern appLayer app;
extern linkLayer llink;
int counter = 0;

void llopenT(int fd);
void llopenR(int fd);

void alarm_handler() {
  tcflush(app.fileDescriptor, TCIOFLUSH);

  if (llink.numTransmissions == counter) {
    printf("Could not connect to receiver. Halting execution...\n");
    if (tcsetattr(app.fileDescriptor, TCSANOW, &llink.oldPortSettings) == -1) {
      perror("tcsetattr");
      close(app.fileDescriptor);
      exit(-1);
    }
    close(app.fileDescriptor);
    exit(-1);
  }
  counter++;
  printf("Connection timed out. Retrying... (attempt number %d) \n",
         counter);
  if(app.status == TRANSMITTER){
    write(app.fileDescriptor, llink.frame, llink.frame_size);
  }
  alarm(llink.timeout);
}

char* getPort(int port) {
  char* path = malloc(10);
  switch (port) {
    case 0:
      strcat(path, COM_1);
      break;
    case 1:
      strcat(path, COM_2);
      break;
    case 2:
      strcat(path, COM_3);
      break;

    default:
      break;
  }

  return path;
}

int setTermios(int fd) {
  struct termios oldtio, newtio;

  if (tcgetattr(fd, &oldtio) == -1) { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  llink.oldPortSettings = oldtio;

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = llink.baudRate | CS8 | CLOCAL | CREAD;
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

  return 0;
}

int llopen(int port, int flag) {
  int fd;
  char* path;

  if(flag != TRANSMITTER && flag != RECEIVER){
    perror("Wrong flag");
    return -1;
  }

  app.status = flag;
  llink.frame_size = 5;

  path = getPort(port);

  fd = open(path, O_RDWR | O_NOCTTY);
  // Checks if port has opened without errors
  if (fd < 0) {
    perror(path);
    exit(-1);
  }
  // Set new termios settings
  if (setTermios(fd) < 0) {
    perror("Setting termios failed");
    return -1;
  }

  signal(SIGALRM, alarm_handler);

  strcpy(llink.port, path);
  free(path);

  app.fileDescriptor = fd;

  switch (flag) {
    case TRANSMITTER:
      llopenT(fd);
      break;
    case RECEIVER:
      llopenR(fd);
      break;
    default:
      
      break;
  }

  return fd;
}

void llopenR(int fd) {
  char uaArr[5];
  char buf[255];
  int res;

  states state = START;

  makeUA(uaArr);


  // Receive SET
  while (1) {
    if (counter == llink.numTransmissions) {
      perror("Exceeded max number of tries. Exiting");
      exit(-1);
    }

    printf("STATE: %d\n", state);

    alarm(llink.timeout);
    for (size_t i = 0; state != STOP; i++) {
      res = read(fd, &buf[i], 1);
      printf("BYTE: %#x\n", buf[i]);
      advance_state_SET(buf[i], &state);
      printf("STATE: %d\n", state);
    }
    alarm(0);

    if ((buf[1] ^ buf[2]) != buf[3] && buf[3] != BCC_SND) {
      bzero(buf, 5);
      counter++;
      continue;
    }

    counter = 0;
    break;
  }

  // Send UA
  res = write(fd, uaArr, 5);
}

void llopenT(int fd) {
  char setArr[5];
  char buf[255];
  int res;

  states state = START;

  makeSET(setArr);

  memcpy(llink.frame,setArr,5);

  while (1) {
    if (counter == llink.numTransmissions) {
      perror("Exceeded max number of tries. Exiting");
      exit(-1);
    }

    // Send SET
    res = write(fd, setArr, 5);
    alarm(llink.timeout);

    printf("STATE: %d\n", state);

    // Receive UA
    for (size_t i = 0; state != STOP; i++) {
      res = read(fd, &buf[i], 1);
      printf("BYTE: %#x\n", buf[i]);
      advance_state_UA(buf[i], &state);
      printf("STATE: %d\n", state);
    }
    alarm(0);

    if ((buf[1] ^ buf[2]) != BCC_RCV) {
      bzero(buf, 5);
      counter++;
      continue;
    }

    counter = 0;
    break;
  }
}

int llwrite(int fd, char* buffer, int length){
  if (length <= 0){
    perror("length 0 or less");
    return -1;
  }



}


