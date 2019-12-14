#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "ftp.h"

int connectSocket(struct ext * ext){
    int fd;
    struct sockaddr_in server_address;

    bzero((char*) &server_address, sizeof(struct sockaddr_in));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ext->ip);
    server_address.sin_port = htons(ext->port);

    fd = socket(AF_INET, SOCK_STREAM, 0);

    if(fd < 0){
        perror("Failed to open socket");
        exit(FAIL);
    }

    if (server_address.sin_addr.s_addr == -1){
      printf("Ip conversion error");
      exit(-1);
    }

    if (connect(fd, (struct sockaddr*)&server_address, sizeof(struct sockaddr)) <
        0) {
      perror("Failed to connect");
      exit(FAIL);
    }

    return fd;
}

int writeToSocket(struct ftp* ftp, char* str, int size){
    int n;

    if((n = write(ftp->control_fd, str, size)) < 0){
        perror("Error writing to socket");
        exit(FAIL);
    }

    return SUCCESS;
}

int readFromSocket(struct ftp* ftp, char*str, size_t size){
    FILE* fd = fdopen(ftp->control_fd, "r");

    do{
        memset(str, 0, size);
        if((str = fgets(str, size, fd)) == NULL){
            perror("Failed to read from socket");
            exit(FAIL);
        }
    }while(str[0] < '1' && str[0] > '5' || str[3] != ' ');

    return SUCCESS;
}

int connectHost(struct ftp* ftp, struct url * url){
    int fd;
    char str[STR_LEN];
    fd = connectSocket(&(url->ext));

    ftp->control_fd = fd;
    ftp->data_fd = 0;
    
    readFromSocket(ftp, str, STR_LEN);

    return SUCCESS;
}

int loginUser(struct ftp * ftp, struct url * url){
    char sd[1024];

	// username
	sprintf(sd, "USER %s\r\n", url->user);
	writeToSocket(ftp, sd, strlen(sd));

	readFromSocket(ftp, sd, sizeof(sd));

	// cleaning buffer
	memset(sd, 0, sizeof(sd));

	// password
	sprintf(sd, "PASS %s\r\n", url->password);
	writeToSocket(ftp, sd, strlen(sd));

    readFromSocket(ftp, sd, sizeof(sd));

	return 0;
}

int changeDirectory(struct ftp * ftp, struct url * url){
    char cwd[STR_LEN];
    int size = strlen(url->path);

    if(size < 0){
        perror("Path incorrect");
        exit(FAIL);
    }
    else if(size > 0){
        sprintf(cwd, "cwd %s\r\n", url->path);
        writeToSocket(ftp, cwd, strlen(cwd));
        readFromSocket(ftp, cwd, STR_LEN);
    }  

    return SUCCESS;
}

int passiveMode(struct ftp * ftp){
    char passive[STR_LEN];

    sprintf(passive, "pasv\r\n");
    writeToSocket(ftp, passive, strlen(passive));
    
    readFromSocket(ftp, passive, STR_LEN);

    int ip1, ip2, ip3, ip4;
	int port1, port2;
	if ((sscanf(passive, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &ip1,&ip2, &ip3, &ip4, &port1, &port2)) < 0) {
		perror("Failed to calculate port");
		exit(FAIL);
	}

	memset(passive, 0, sizeof(passive));
    
	if ((sprintf(passive, "%d.%d.%d.%d", ip1, ip2, ip3, ip4)) < 0) {
		perror("Failed to form ip address.");
		exit(FAIL);
	}

	//calculate new port
	int newPort= port1 * 256 + port2;

    struct ext * ext = malloc(sizeof(struct ext));
    ext->ip = passive;

    ext->port = newPort;

    ftp->data_fd = connectSocket(ext);

	return SUCCESS;
}

int retrieve(struct ftp* ftp, struct url * url){
    char retrieve[STR_LEN];

    sprintf(retrieve, "RETR %s\r\n", url->path);
    writeToSocket(ftp, retrieve, strlen(retrieve));
    readFromSocket(ftp, retrieve, STR_LEN);

    return SUCCESS;
}

int download(struct ftp* ftp, struct url * url){
    FILE* fd;
	int n;
    char buffer[STR_LEN];

    if (!(fd = fopen(url->file, "w"))) {
		perror("Failed to open file in download.\n");
		exit(FAIL);
	}

    while ((n = read(ftp->data_fd, buffer, STR_LEN)) > 0)
    {
        if ((n = fwrite(buffer, 1, n, fd)) < 0) {
			perror("Failed to write data on dowload");
			exit(FAIL);
		}
    }

    fclose(fd);
	close(ftp->data_fd);

	return SUCCESS;
}

int disconnect(struct ftp* ftp) {
	char disconnect[STR_LEN];

	readFromSocket(ftp, disconnect, STR_LEN);

	sprintf(disconnect, "QUIT\r\n");
	writeToSocket(ftp, disconnect, strlen(disconnect));

	close(ftp->control_fd);

	return SUCCESS;
}