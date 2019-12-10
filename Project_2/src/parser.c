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
        puts("URL header incorrect (should be ftp://");
        return (-1);
    }
} 

/**
 * Checks if username is declared in the URL.
 * Returns the index of the ':' char.
 */
int checkUser(char *url) {
    // Username starts after ftp://, that is, on index 6
    int userStartIndex = 6;
    // Counter for the number of chars that are part of the username
    int counter = 0;
    for (unsigned int i = userStartIndex; url[i] != ':'; i++) {
        counter++;
    }
    // If username not declared, return with error code
    if (counter <= 0) {
        puts("Tá mal");
        return -2;
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
        puts("Username and password must be separated with ':' char");
        return -3;
    }
    
    for (unsigned int i = startIndex; url[i] != '@'; i++) {
        counter++;
    }
    
    // If username not declared, return with error code
    if (counter <= 0) {
        puts("Username not declared");
        return -2;
    }

    return counter;
}

/**
 * Chekcs if host is declared in the URL.
 * Returns index of the '/' character on success.
 */
int checkHost(const char* url, int startIndex) {
    int i, url_length = strlen(url), hostDeclared = 0;
    for (i = startIndex; i < url_length; ++i) {
        if (url[i] == '/') {
            hostDeclared = 1;
            break;
        }
    }

    if (!hostDeclared) {
        printf("No host declared in the URL");
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
        printf("No path declared in the URL");
        exit(-5);
    }

    return i;
}

char* getFileName(char* path) {
    int i = strlen(path);
    for (i = i - 1; path[i] != '/'; i--);
    char* file = strndup(path + i + 1, strlen(path) - 1);

    return file;
}

/**
 * Returns 1 if parse was valid; 0 otherwise.
 */
//printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
int parseURL(char* url, char* user, char* password, char* host, char* path, char* file) {
    checkURLheader(url);
    int passwordStartIndex = checkUser(url) + 1; // +1 (char next to ':')
    int hostStartIndex = passwordStartIndex + checkPassword(url, passwordStartIndex) + 1; // +1 (char next to '@')
    int pathStartIndex = checkHost(url, hostStartIndex) + 1;
    checkPath(url, pathStartIndex);
    user = strndup(url + 6, passwordStartIndex - 7);
    password = strndup(url + passwordStartIndex, hostStartIndex - passwordStartIndex - 1);
    host = strndup(url + hostStartIndex, pathStartIndex - hostStartIndex - 1);
    path = strndup(url+ pathStartIndex, strlen(url) - pathStartIndex - 1);
    file = getFileName(path);

    printf("%s\n", file);

    return 0;
}

int main() {
    char* url = "ftp://gustus:loucura@feup/sfgdhjf/hbefrghjtrkn/";
    char* user, password, host, path, file;
    parseURL(url, user, &password, &host, &path, &file);
    return 0;
}