#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Checks if the user and password were given by searching for the @ sign
 *
 * @param url
 * @return int
 */
int checkOptional(char *url) {
    int optional = 0;
    int FOUND_AT = 0;
    int FOUND_DOTDOT = 0;

    for (size_t i = 0; url[i] != '\0'; i++) {
        if (url[i] == ':' && !FOUND_DOTDOT) {
            if (url[i + 1] == '@') {
                printf("Password can't be empty.\n");
                exit(-4);
            }
            optional += 1;
            FOUND_DOTDOT = 1;
        }
        if (url[i] == '@' && !FOUND_AT) {
            if (url[i + 1] == '\0') {
                printf("Host can't be empty.\n");
                exit(-5);
            }
            optional += 1;
            FOUND_AT = 1;
        }
    }

    if ((FOUND_DOTDOT && !FOUND_AT)) {
        printf("You're missing an @.\n");
        exit(-3);
    }

    if (optional >= 3) {
        printf("You can have only one @ or :.\n");
        exit(-2);
    }

    return optional;
}

void checkEmptyURL(char *url) {
    char aux[0] = "";
    if (strcmp(url, aux) == 0) {
        printf("URL cannot be an empty string\n");
        exit(-1);
    }
}

/**
 * Checks if URL starts with ftp://
 */
int checkURLheader(char *url) {
    char header[6] = "ftp://";
    // Compares the first 6 chars of the URL, to check if
    // they are the same as the header
    if (strncmp(url, header, 6) == 0)
        return 0;
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

    for (unsigned int i = userStartIndex; url[i] != ':' && url[i] != '\0'; i++) {
        counter++;
    }
    // If username not declared, return special -90 value
    if (counter <= 0) {
        return -90;
    }

    // printf("%d\n",userStartIndex + counter);

    return userStartIndex + counter;
}

/**
 * Checks if password exists in the URL. If it does.
 * Returns the index of the '@' char.
 */
int checkPassword(char *url, int startIndex) {
    int counter = 0;

    for (unsigned int i = startIndex; url[i] != '@' && url[i] != '\0'; i++) {
        if (url[i] == '\0')
            printf("Encontrei-te puta %d \n", counter);
        counter++;
    }

    // If username not declared, return special -91 value
    if (counter <= 0) {
        return -91;
    }

    return counter;
}

/**
 * Checks if host is declared in the URL.
 * Returns index of the '/' character on success.
 */
int checkHost(const char *url, int startIndex) {
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

int checkPath(const char *url, int startIndex) {
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

int getFileNameIndex(char *path) {
    const size_t str_size = strlen(path);
    size_t i = str_size;
    for (; i > 0; --i) {
        if (path[i] == '/') {
            return i + 1;
        }
    }
    return 0;
}

void askPassword(char *pwd) {
    printf("Enter your password: ");
    scanf("%s", pwd);
}

void askUser(char *user) {
    printf("Enter your username: ");
    scanf("%s", user);
}

/**
 * Returns 1 if parse was valid; 0 otherwise.
 */
// printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
int parseURL(char *url, struct url *result) {
    int MANUALLY_INSERTED_USERNAME = 0, MANUALLY_INSERTED_PASSWORD;
    char *user = malloc(sizeof(char) * 24);
    strcpy(user, "anonymous");
    char *password = malloc(sizeof(char) * 24);
    strcpy(password, "anonymous");
    int urlsize = strlen(url);
    const int START_INDEX =
            6; // "ftp://" has 6 char therefore the parse should start from position
    // after the last '/' aka position no. 6
    const int OPTIONAL = checkOptional(url + 6);

    checkEmptyURL(url);
    checkURLheader(url);

    // Optional User is given
    switch (OPTIONAL) {
        case 0:
            break;
        case 1: // Ask for password
            //askPassword(password);
            MANUALLY_INSERTED_PASSWORD = 1;
            printf("AQUI\n");
            break;
        case 2: // Ask for password and user
            //askUser(user);
            //askPassword(password);
            MANUALLY_INSERTED_USERNAME = 1;
            MANUALLY_INSERTED_PASSWORD = 1;
            printf("ALI\n");
            break;
        default: // Case 0 everything is on it, greater or lesser than 0 the
            // program exits
            MANUALLY_INSERTED_USERNAME = 1;
            MANUALLY_INSERTED_PASSWORD = 1;
            printf("ACOLÁ\n");
            exit(-5);
    }

    int passwordStartIndex = (checkUser(url) + 1); // +1 (char next to ':')
    passwordStartIndex =
            (passwordStartIndex >= urlsize || passwordStartIndex < 0) ? START_INDEX : passwordStartIndex;

    int hostStartIndex;
    int pathStartIndex;
    if (OPTIONAL == 0) {
        hostStartIndex = 6;
        pathStartIndex = checkHost(url, hostStartIndex) + 1;
    } else {
        hostStartIndex = passwordStartIndex + checkPassword(url, passwordStartIndex) + 1;
        pathStartIndex = checkHost(url, hostStartIndex) + 1;
    }

    checkPath(url, pathStartIndex);
    //   result->user = strndup(url + 6, passwordStartIndex - 7);
    //result->user = user;

    if (OPTIONAL == 0) {
        result->user = malloc(sizeof(char) * 24);
        result->password = malloc(sizeof(char) * 24);
        strcpy(result->user, "anonymous");
        strcpy(result->password, "anonymous");
    }
    else if (OPTIONAL == 1) {
        result->user = strndup(url + 6, hostStartIndex - 7);
        result->password = malloc(sizeof(char) * 24);
        askPassword(result->password);
    }
    else if (OPTIONAL == 2) {
        result->user = strndup(url + 6, passwordStartIndex - 7);
        result->password = strndup(url + passwordStartIndex, hostStartIndex - passwordStartIndex - 1);
    }


    //result->password = password;
    result->host = strndup(url + hostStartIndex, pathStartIndex - hostStartIndex - 1);

    result->path = strndup(url + pathStartIndex, strlen(url) - pathStartIndex);

    int fileNameIndex = getFileNameIndex(result->path);
    if (fileNameIndex == 0) {
        result->file = result->path;
    } else
        result->file = strndup(result->path + fileNameIndex, strlen(result->path));
    if (*(result->file) == '\0') {
        puts("No file declared in the url\n");
        exit(-5);
    }

    return 0;
}

int askforUser(struct url *url) { return 0; }