#include <stdio.h>
#include "parser.h"

int main() {
    char* url = "ftp://eergr:loucura@rgtr/rfetg/";
    char* user, password, host, path, file;
    parseURL(url, user, &password, &host, &path, &file);
    return 0;
}
