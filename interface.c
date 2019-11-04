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

#include "applicationLayer.h"
#include "llmacros.h"
#include "packet_factory.h"
#include "state_machine.h"

extern appLayer app;
extern linkLayer llink;
int counter = 0;
unsigned long byte_counter = 0;

void llopenT(int fd);
void llopenR(int fd);
void llcloseT(int fd, int flag);
void llcloseR(int fd, int flag);

void alarm_handler() {
  tcflush(app.fileDescriptor, TCIOFLUSH);

  if (llink.numTransmissions == counter) {
    perror("Could not connect to receiver. Halting execution...\n");
    if (tcsetattr(app.fileDescriptor, TCSANOW, &llink.oldPortSettings) == -1) {
      perror("tcsetattr");
      close(app.fileDescriptor);
      exit(-1);
    }
    close(app.fileDescriptor);
    exit(-1);
  }
  counter++;
  printf("Connection timed out. Retrying... (attempt number %d) \n", counter);
  if (app.status == TRANSMITTER) {
    tcflush(app.fileDescriptor, TCIOFLUSH);
    write(app.fileDescriptor, llink.frame, llink.frame_size);
  }
  alarm(llink.timeout);
}

int writepacket(int fd){
  int res = 0;
  for (size_t i = 0; i < llink.frame_size; i++) {
    res += write(fd, llink.frame + i, 1);
  }
  return res;
}

char* getPort(int port) {
  char* path = malloc(10);
  switch (port) {
    case 0:
      strcat(path, "/dev/ttyS0");
      break;
    case 1:
      strcat(path, "/dev/ttyS1");
      break;
    case 2:
      strcat(path, "/dev/ttyS2");
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
  int fd;  // serial port file
  char* path;

  if (flag != TRANSMITTER && flag != RECEIVER) {
    perror("Wrong flag");
    return -1;
  }

  app.status = flag;
  llink.frame_size = 5;
  llink.sequenceNumber = 0;

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
  unsigned char uaArr[5];
  unsigned char buf[MAX_FRAME_SIZE];
  int res;

  states state = START;

  makeUA(uaArr);

  // Receive SET
  while (1) {
    if (counter == llink.numTransmissions) {
      perror("Exceeded max number of tries. Exiting");
      exit(-1);
    }
    for (size_t i = 0; state != STOP; i++) {
      i = (state == START) ? 0 : i;
      res = read(fd, &buf[i], 1);
      advance_state_SET(buf[i], &state);
    }
    alarm(0);

    if (((buf[1] ^ buf[2]) != buf[3]) &&
        (buf[3] !=
         (A_SND ^ C_SET)))  // Verifies if the sender's BCC is not valid
    {
      bzero(buf, 5);
      continue;
    }

    counter = 0;
    break;
  }

  // Send UA
  res = write(fd, uaArr, 5);
}

void llopenT(int fd) {
  unsigned char setArr[5];
  unsigned char buf[MAX_FRAME_SIZE];
  int res;

  states state = START;

  makeSET(setArr);

  memcpy(llink.frame, setArr, 5);

  while (1) {
    if (counter == llink.numTransmissions) {
      perror("Exceeded max number of tries. Exiting");
      exit(-1);
    }

    // Send SET
    res = write(fd, setArr, 5);

    // Receive UA
    alarm(llink.timeout);
    for (size_t i = 0; state != STOP; i++) {
      i = (state == START) ? 0 : i;
      res = read(fd, &buf[i], 1);
      advance_state_UA(buf[i], &state);
    }
    alarm(0);

    if (((buf[1] ^ buf[2]) != buf[3]) &&
        (buf[3] != (A_RCV ^ C_UA)))  // verifies if receiver's Bcc is not valid
    {
      bzero(buf, 5);
      continue;
    }

    counter = 0;
    break;
  }
}

int llwrite(int fd, unsigned char* buffer, int length) {
  if (length <= 0) {
    perror("length 0 or less");
    return -1;
  }

  makePacket(buffer, length, llink.sequenceNumber);
  counter = 0;

  while (1) {
    unsigned char buf[MAX_FRAME_SIZE * 3], bcc;
    int res;
    states state = START;

    if (counter == llink.numTransmissions) {
      perror("Exceeded max number of tries. Exiting");
      exit(-1);
    }

    size_t i;
    control_t Spacket;

    // Send I packet
    tcflush(app.fileDescriptor, TCIOFLUSH);
    usleep(TPROP);    
    res = writepacket(fd);

    // Receive RR or REJ
    alarm(llink.timeout);
    for (i = 0; state != STOP; i++) {
      i = (state == START) ? 0 : i;
      res = read(fd, &buf[i], 1);
        byte_counter++;      
        advance_state_RR(buf[i], &state);
    }
    alarm(0);

    bcc = buf[3];

    // Check BCC
    if (buf[3] != (buf[2] ^ buf[1]))  // verifies if receveir's Bcc is not valid
    {
      continue;
    }

    Spacket = getPacketType(buf);

    switch (Spacket) {
      case RR:
        if ((llink.sequenceNumber && buf[3] == C_RR0) ||
            (!llink.sequenceNumber && buf[3] == C_RR1)) {
          res = llink.frame_size;
        }
        break;
      case REJ:
		printf("Reject Packet\n");
        continue;
      default:
        return -1;
        break;
    }

    llink.sequenceNumber = !llink.sequenceNumber;
    return res;
  }
}

/*
no llread vai ser necessario adicionar uma parte à maquina de estados, para caso
C seja C_DISC, verifica, a BCC e se a seguir vem a flag, e neste caso chama a
funçao receivedDISCframeRCV
*/
int llread(int fd, unsigned char* buffer) {
  counter = 0;

  while (1) {
    int packet_size = 0;
    int i = 0;
    int disc = 0;  // bool to see if it received a DISC packet
    unsigned char header[5];
    unsigned char buf[MAX_FRAME_SIZE];
    unsigned char* data_packet = malloc(1);
    int res, bcc_correct;
    states state = START;
    unsigned char bcc2 = 0;

    packet_size = 0;
    int destuf_buf_size = 0;
    // Read packet
    for (i = 0; state != STOP; i++) {
      i = (state == START) ? 0 : i;
      if(i == MAX_FRAME_SIZE){
		makeREJ(header, !llink.sequenceNumber);       
        res = write(fd, header, 5);
		printf("BCC1 Reject Packet\n");
        continue;
	  }
        byte_counter++;
      res = read(fd, &buf[i], 1);

      advance_state_I(buf[i], &state, &disc);

      if (state == DATA_R) {
        data_packet = (unsigned char*)realloc(data_packet, packet_size + 1);
        data_packet[packet_size] = buf[i];
        packet_size++;
      }
    }

    unsigned char destuf_buf[packet_size];
    if (disc) {
      llcloseR(fd, RECEIVER);
      return 0;
    }
    else {
      destuf_buf_size = destuffing(data_packet, packet_size, destuf_buf);

      bcc2 = destuf_buf[destuf_buf_size - 1];
      bcc_correct = checkBcc2(destuf_buf, destuf_buf_size - 1, bcc2);

    // Checks if bcc2 is correct
      if (bcc_correct) {
        // send RR
        makeRR(header, !llink.sequenceNumber);
      } else {
        // send REJ
        makeREJ(header, !llink.sequenceNumber);
        res = write(fd, header, 5);
		printf("BCC2 Reject Packet\n");
        continue;
      }

      res = write(fd, header, 5);
      memcpy(buffer, destuf_buf, destuf_buf_size - 1);
      llink.sequenceNumber = !llink.sequenceNumber;
      free(data_packet);
      return (bcc_correct) ? destuf_buf_size : -1;
    }
  }
}

int llclose(int fd, int flag) {
  counter = 0;
  if (flag == TRANSMITTER)
    llcloseT(fd, flag);
  if (flag == RECEIVER)
    llcloseR(fd, flag);

  sleep(1);
  tcsetattr(fd, TCSANOW, &llink.oldPortSettings);
  close(fd);
  return 0;
}

void llcloseT(int fd, int flag) {
  // fd identificador da ligação de dados
  puts("Entered llclose");
  
  unsigned char discArray[5] = {FLAG, A_SND, C_DISC, A_SND ^ C_DISC, FLAG};
  unsigned char uaArray[5] = {FLAG, A_SND, C_UA, (A_SND ^ C_UA), FLAG};
  unsigned char buffer[MAX_FRAME_SIZE];
  int res;

  states disc_state = START;
  counter = 0;

  memcpy(llink.frame, discArray, 5);

  while (1) {
    if (counter == llink.numTransmissions) {
      perror("Exceeded max number of tries. Exiting");
      exit(-1);
    }
    // Send DISC
    res = write(fd, discArray, 5);

    // WAIT FOR DISC
    alarm(llink.timeout);
    for (unsigned int i = 0; disc_state != STOP; i++) {
      i = (disc_state == START) ? 0 : i;
      res = read(fd, &buffer[i], 1);
      advance_state_DISC(buffer[i], &disc_state);
    }
    alarm(0);

    // if message is corrupted
    if ((buffer[1] ^ buffer[2]) != (buffer[3])) {
      bzero(buffer, 5);
      continue;
    }

    counter = 0;
    break;
  }

  // SEND UA
  write(fd, uaArray, 5);
}
// Closes RCV function before ending of times and we all pass RCOM
void llcloseR(int fd, int flag) {
  puts("Entered llclose");
  
  unsigned char buffer[256];
  unsigned char discArray[5] = {FLAG, A_RCV, C_DISC, (A_RCV ^ C_DISC), FLAG};

  int res;

  states disc_state = START, ua_state = START;

  write(fd, discArray, 5);
  memcpy(llink.frame, discArray, 5);
  // Read UA
  while (1) {
    if (counter == llink.numTransmissions) {
      perror("Exceeded max number of tries. Exiting");
      exit(-1);
    }
    alarm(llink.timeout);
    for (unsigned int i = 0; ua_state != STOP; i++) {
      i = (ua_state == START) ? 0 : i;
      res = read(fd, &buffer[i], 1);
      advance_state_UA_DISC(buffer[i], &ua_state);
    }
    alarm(0);

    if ((buffer[1] ^ buffer[2]) != (buffer[3])) {
      bzero(buffer, 5);
      continue;
    }

    counter = 0;
    break;
  }
}
