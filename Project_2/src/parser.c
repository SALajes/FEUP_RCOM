#include <stdio.h>
#include <string.h>

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
    return counter;
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
        puts("Tá mal");
        return -2;
    }

    return counter;
}

/**
 * Returns 1 if parse was valid; 0 otherwise.
 */
//printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
int parseURL(char* url, char* user, char* password, char* host, char* path, char* file) {
    checkURLheader(url);
    int passwordStartIndex = checkUser(url) + 2;

    return 0;
}


int main() {
    char* url = "ftp://gustus:loucura@feup/berf";
    checkPassword(url, 13);
}