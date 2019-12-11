#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * Checks if URL starts with ftp://
 */
int checkURLheader(char *url) {
    char header[6] = "ftp://";
    // Compares the first 6 chars of the URL, to check if
    // they are the same as the header
    if (strncmp(url, header, 6) == 0) return 0;
    else {
        puts("URL header incorrect (should be ftp://)\n");
        exit(-1);
    }
} 

/**
 * Checks if username is declared in the URL.
 * Returns the index of the ':' char.
 */
int checkUser(char *url) {
    if (strlen(url) < 6) {
        puts("URL not valid");
        exit(-10);
    }
    // Username starts after ftp://, that is, on index 6
    int userStartIndex = 6;
    // Counter for the number of chars that are part of the username
    int counter = 0;
    for (unsigned int i = userStartIndex; url[i] != ':'; i++) {
        counter++;
    }
    // If username not declared, return special -90 value
    if (counter <= 0) {
        return -90;
    }
    return userStartIndex + counter;
}

/**
 * Checks if password exists in the URL. If it does. 
 * Returns the index of the '@' char.
 */
int checkPassword(char *url, int startIndex) {
    int counter = 0;

    if (url[startIndex - 1] != ':') {
        puts("Username and password must be separated with ':' char\n");
        exit(-3);
    }
    
    for (unsigned int i = startIndex; url[i] != '@'; i++) {
        counter++;
    }
    
    // If username not declared, return special -91 value
    if (counter <= 0) {
        return -91;
    }

    return counter;
}

/**
 * Chekcs if host is declared in the URL.
 * Returns index of the '/' character on success.
 */
int checkHost(const char* url, int startIndex) {
    int i, url_length = strlen(url), hostDeclared = 0, count = 0;
    for (i = startIndex; i < url_length; ++i) {
        count++;
        if (url[i] == '/') {
            hostDeclared = 1;
            break;
        }
    }

    if ((hostDeclared == 0 && count == 0) || (hostDeclared == 1 && count == 0)) {
        printf("No host declared in the URL\n");
        exit(-4);
    }

    return i;
}

int checkPath(const char* url, int startIndex) {
    int i = startIndex, url_length = strlen(url), counter = 0;
    for (i; i < url_length; i++) {
        counter++;
    }

    if (counter == 0) {
        printf("No path declared in the URL\n");
        exit(-5);
    }

    return i;
}

int getFileNameIndex(char* path) {
    const size_t str_size = strlen(path);
    size_t i = str_size;
    for (; i > 0; --i) {
        if (path[i] == '/') {
            return i + 1;
        }
    }
    return 0;
}

/**
 * Returns 1 if parse was valid; 0 otherwise.
 */
//printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
int parseURL(char* url, char* user, char* password, char* host, char* path, char* file) {
    checkURLheader(url);
    int passwordStartIndex = checkUser(url) + 1; // +1 (char next to ':')
    int hostStartIndex = passwordStartIndex + checkPassword(url, passwordStartIndex) + 1;
    int pathStartIndex = checkHost(url, hostStartIndex) + 1;
    checkPath(url, pathStartIndex);
    user = strndup(url + 6, passwordStartIndex - 7);
    password = strndup(url + passwordStartIndex, hostStartIndex - passwordStartIndex - 1);
    host = strndup(url + hostStartIndex, pathStartIndex - hostStartIndex - 1);
    path = strndup(url+ pathStartIndex, strlen(url) - pathStartIndex - 1);

    int fileNameIndex = getFileNameIndex(path);
    if (fileNameIndex == 0) {
        file = path;
    }
    else file = strndup(path + fileNameIndex, strlen(path));
    if (*file == '\0') {
        puts("No file declared in the url\n");
        exit(-5);
    }

    return 0;
}

int main() {
    char* url = "ftp://eergr:loucura@rgtr/rfetg/";
    char* user, password, host, path, file;
    parseURL(url, user, &password, &host, &path, &file);
    return 0;
}