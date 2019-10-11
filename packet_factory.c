#include "packet_factory.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "llmacros.h"

extern appLayer app;
extern linkLayer llink;

int stuffing(const char* array,size_t size, char* aux_array) {
  unsigned int i, j;
  for (i = 0, j = 0; i < size; i++, j++) {
    switch (array[i]) {
      case FLAG_RCV:
        aux_array[j] = ESC_OCT;
        aux_array[j + 1] = FLAG_STF;
        j++;
        break;
      case ESC_OCT:
        aux_array[j] = ESC_OCT;
        aux_array[j + 1] = ESC_STF;
        j++;
        break;
      default:
        aux_array[j] = array[i];
    }
  }
  return j;
}

int destuffing(const char* array, size_t size, char* aux_array) {
  unsigned int i, j;
  for (i = 0, j = 0; i < size; i++, j++) {
    if(array[i] == ESC_OCT){
      switch (array[i+1])
      {
      case FLAG_STF:
        aux_array[j] = FLAG_OCT;
        i++;
        break;
      case ESC_STF:
        aux_array[j] = ESC_OCT;
        i++;
        break;
      default:
        break;
      }
    }else
      aux_array[j] = array[i];
    
  }
  return j;
}

void makeUA(char* uaarr) {
  uaarr[0] = FLAG_RCV;
  uaarr[1] = A_RCV;
  uaarr[2] = C_RCV;
  uaarr[3] = BCC_RCV;
  uaarr[4] = FLAG_RCV;
}

char* makeBcc(const char* data_field) {
  char* Bcc = malloc(1);

  Bcc[0] = data_field[0];

  for (int i = 1; i < STRSIZE; i++) {
    Bcc[0] = Bcc[0] ^ data_field[i];
  }

  return Bcc;
}

void makeSET(char* setarr) {
  if(setarr == NULL) return;
  setarr[0] = FLAG_RCV;
  setarr[1] = A_SND;
  setarr[2] = C_SND;
  setarr[3] = BCC_SND;
  setarr[4] = FLAG_RCV;
}

void makeREJ(char* REJarr,int s) {
  if (REJarr == NULL) return;
  REJarr[0] = FLAG_RCV;
  REJarr[1] = A_RCV;
  REJarr[2] = (s) ? C_REJ1 : C_REJ0;
  REJarr[3] = BCC_RCV;
  REJarr[4] = FLAG_RCV;
}

void makeRR(char* RRarr, int s){
  if(RRarr == NULL) return;
  RRarr[0] = FLAG_RCV;
  RRarr[1] = A_RCV;
  RRarr[2] = (s) ? C_RR1 : C_RR0;
  RRarr[3] = A_RCV ^ RRarr[2];
  RRarr[4] = FLAG_RCV;
}

void makePacket(const char* data_field, size_t size, int s) {
  char packet[size * 3];
  char* aux_array = malloc(size * 2); 
  int res = 0;
  int stf_size;

  bzero(packet,size*3);

  //Header
  packet[0] = FLAG_OCT;
  packet[1] = A_SND;
  packet[2] = (s) ? C_I1 : C_I0;
  packet[3] = A_SND ^ packet[2];

  res += 3;

  // Data field
  stf_size = stuffing(data_field, size, aux_array);
  memcpy(packet + res, aux_array,stf_size);
  res += stf_size;

  // Bcc for data
  memcpy(packet + res, makeBcc(data_field),1);
  res++;

  char flag[1] = {FLAG_OCT};
  memcpy(packet + res , flag, 1);
  res++;

  llink.frame_size = res;
  memcpy(llink.frame,packet,res);
}

control_t what_Spacket(unsigned char* packet) {
  unsigned char c = packet[2];
  if (c == C_SND)
    return SET;
  if (c == C_DISC)
    return DISC;
  if (c == C_RCV)
    return UA;
  if (c == C_RR0 || c == C_RR1)
    return RR;
  if (c == C_REJ0 || c == C_REJ1)
    return REJ;
  return CPRT;
}
