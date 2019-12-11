#pragma once

#include "url.h"

int parseURL(char* url, struct url* result);
int askforUser(struct url* url);
