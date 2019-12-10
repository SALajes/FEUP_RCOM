#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        exit(1);
    }

    char* user = NULL, password = NULL, host = NULL, path = NULL, file = NULL, ip = NULL;

    parseURL(argv[1], &user, &password, &host, &path, &file);

    free(user);
    free(password);
    free(host);
    free(ip);
    free(path);
    free(file);

    return 0;
}