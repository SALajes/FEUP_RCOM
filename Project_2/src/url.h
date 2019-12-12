#ifndef _URL_H
#define _URL_H

struct ext
{
    char* ip;
    int port;
};

struct url
{
    char* user;
    char* password;
    char* host;
    char* path;
    char* file;
    struct ext ext;
};


#define SUCCESS 0
#define FAIL    1

#define STR_LEN 512

#endif