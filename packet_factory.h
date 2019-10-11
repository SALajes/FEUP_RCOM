#ifndef PACKET_FACTORY
#define PACKET_FACTORY
#include <sys/types.h>

void makePacket(const char* header, const char* data_field, size_t size_t);

void makeSET(char* setarr);
void makeUA(char* uaarr);

#endif