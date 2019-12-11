
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

#include "ftp.h"
#include "parser.h"

#define SUCCESS 0
#define FAIL    1


void getIP(struct url* url);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        exit(FAIL);
    }

    struct url * url = malloc(sizeof(struct url));
    url->ext->port = 21;

    parseURL(argv[1], url);

// TO DO: TRATAR DO ANONIMOUS
    // if(url->user == NULL)
    //     askforUser(url);   

    getIP(url);

    //FTP
    struct ftp* ftp;

    connectHost(ftp, url);
    loginUser(ftp, url);
    changeDirectory(ftp, url);
    passiveMode(ftp);
    download(ftp, url);
    disconnect(ftp);

    free(url);

    return 0;
}

void getIP(struct url* url){
    struct hostent* h = gethostbyname(url->host);

    if(h == NULL){
        perror("Hostent is null\n");
        exit(1);    
    }

    char * aux = inet_ntoa(*((struct in_addr *) h->h_addr_list[0]));

    url->ext->ip = malloc(strlen(aux) + 1);

    memcpy(url->ext->ip, aux, strlen(aux));
}
