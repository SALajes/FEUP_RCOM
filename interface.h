#ifndef LLINKINTERFACE
#define LLINKINTERFACE

int llopen(int port, int flag);
int llwrite(int fd, char *buffer, int length);
int llread(int fd, char *buffer);
int llclose(int fd, int flag);

#endif