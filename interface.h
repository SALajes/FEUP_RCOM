void alarm_handler();
char *getPort(int port);
int setTermios(int fd);
int llopen(int port, int flag);
int llwrite(int fd, char *buffer, int length);
int llread(int fd, char *buffer);
int llclose(int fd, int flag);