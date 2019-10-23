#ifndef PACKET_FACTORY
#define PACKET_FACTORY
#include <sys/types.h>
#include "llmacros.h"

void makePacket(const unsigned char *data_field, size_t size_t, int s);

int destuffing(const unsigned char *array, size_t size, char *aux_array);
void makeSET(unsigned char *setarr);
void makeUA(unsigned char *uaarr);
void makeREJ(unsigned char *REJarr, int s);
void makeRR(unsigned char *RRarr, int s);
int checkBcc2(const unsigned char *data_field, int size, unsigned char bcc);
control_t getPacketType(unsigned char *packet);

#endif