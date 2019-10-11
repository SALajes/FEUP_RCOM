#ifndef PACKET_FACTORY
#define PACKET_FACTORY
#include <sys/types.h>
#include "llmacros.h"


void makePacket(const char* data_field, size_t size_t,int s);

void makeSET(char* setarr);
void makeUA(char* uaarr);
void makeREJ(char* REJarr, int s);
void makeRR(char* RRarr, int s);
control_t what_Spacket( unsigned char* packet);

#endif