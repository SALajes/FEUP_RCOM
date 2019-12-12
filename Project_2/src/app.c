
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ftp.h"
#include "parser.h"

#define SUCCESS 0
#define FAIL 1

void getIP(struct url* url);

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
    exit(FAIL);
  }

  struct url url;

  url.ext.port = 21;

  printf("%s\n", argv[1]);

  parseURL(argv[1], &url);

  getIP(&url);

  // FTP
  struct ftp ftp;

  if (connectHost(&ftp, &url) != 0) {
    printf("Something went wrong connecting with the host.\n");
    exit(-1);
  };
  printf("connected\n");
  if (loginUser(&ftp, &url) != 0) {
    printf("Something went wrong logging in.\n");
    exit(-1);
  };
  printf("logged\n");
  if (changeDirectory(&ftp, &url) != 0) {
    printf("Something went wrong logging in.\n");
    exit(-1);
  };
  printf("changed directory\n");
  if (passiveMode(&ftp) != 0) {
    printf("Something went wrong logging in.\n");
    exit(-1);
  };
  
  printf("Passive mode\n");
  download(&ftp, &url);
  disconnect(&ftp);

  return 0;
}

void getIP(struct url* url) {
    printf("host: %s\n", url->host);
  struct hostent* h = gethostbyname(url->host);

  if (h == NULL) {
    perror("Hostent is null\n");
    exit(1);
  }

  char* aux = inet_ntoa(*((struct in_addr*)h->h_addr_list[0])); 

  url->ext.ip = malloc(strlen(aux) + 1);

  memcpy(url->ext.ip, aux, strlen(aux));
}
