#pragma once

#include "url.h"

int parseURL(char* url, struct url* result);
int checkOptional(char* url);
void checkEmptyURL(char *url);
int checkURLheader(char* url);
int checkUser(char* url);
int checkPassword(char* url, int startIndex);
int checkHost(const char* url, int startIndex);
int checkPath(const char* url, int startIndex);
int getFileNameIndex(char* path);
int parseURL(char* url, struct url* result);
int askforUser(struct url* url);
