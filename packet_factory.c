#include "packet_factory.h"
#include "applicationLayer.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "llmacros.h"

#define BYTE_LEN 1

extern appLayer app;
extern linkLayer llink;

int stuffing(const unsigned char *array, size_t size, char *aux_array)
{
  unsigned int i, j;
  for (i = 0, j = 0; i < size; i++, j++)
  {
    switch (array[i])
    {
    case FLAG:
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

int destuffing(const unsigned char *array, size_t size, char *aux_array)
{
  unsigned int i, j;
  for (i = 0, j = 0; i < size; i++, j++)
  {
    if (array[i] == ESC_OCT)
    {
      switch (array[i + 1])
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
    }
    else
      aux_array[j] = array[i];
  }
  return j;
}

void makeUA(unsigned char *uaarr)
{
  uaarr[0] = FLAG;
  uaarr[1] = A_RCV;
  uaarr[2] = C_UA;
  uaarr[3] = A_RCV ^ C_UA;
  uaarr[4] = FLAG;
}

unsigned char *makeBcc(const unsigned char *data_field, int size)
{
  unsigned char *Bcc = malloc(1);

  Bcc[0] = data_field[0];

  for (int i = 1; i < size; i++)
  {
    Bcc[0] = Bcc[0] ^ data_field[i];
  }

  return Bcc;
}

int checkBcc2(const unsigned char *data_field, int size, unsigned char bcc)
{
  if (data_field == NULL)
  {
    return false;
  }

  unsigned char new_bcc;

  new_bcc = data_field[0];

  for (size_t i = 1; i < size; i++)
  {
    new_bcc = new_bcc ^ data_field[i];
  }

  return new_bcc == bcc;
}

void makeSET(unsigned char *setarr)
{
  if (setarr == NULL)
    return;
  setarr[0] = FLAG;
  setarr[1] = A_SND;
  setarr[2] = C_SET;
  setarr[3] = A_SND ^ C_SET;
  setarr[4] = FLAG;
}

void makeREJ(unsigned char *REJarr, int s)
{
  if (REJarr == NULL)
    return;
  REJarr[0] = FLAG;
  REJarr[1] = A_RCV;
  REJarr[2] = (s) ? C_REJ1 : C_REJ0;
  REJarr[3] = A_RCV ^ REJarr[2];
  REJarr[4] = FLAG;
}

void makeRR(unsigned char *RRarr, int s)
{
  if (RRarr == NULL)
    return;
  RRarr[0] = FLAG;
  RRarr[1] = A_RCV;
  RRarr[2] = (s) ? C_RR1 : C_RR0;
  RRarr[3] = A_RCV ^ RRarr[2];
  RRarr[4] = FLAG;
}

/** Change name from packet to frame */
void makePacket(const unsigned char *data_field, size_t size, int sequence_number)
{
  char packet[size * 3];
  char *aux_array = malloc(size * 2);
  int res = 0;
  int stf_size;
  char *bcc2_aux = malloc(2);

  bzero(packet, size * 3);

  //Header
  packet[0] = FLAG_OCT;
  packet[1] = A_SND;
  packet[2] = (sequence_number) ? C_I1 : C_I0;
  packet[3] = A_SND ^ packet[2];

  res += 4;

  // Data field
  stf_size = stuffing(data_field, size, aux_array);
  memcpy(packet + res, aux_array, stf_size);
  res += stf_size;

  // Bcc for data
  // packet[res] = 0; // use this to put bcc2 with error
  bcc2_aux[0] = *makeBcc(data_field, size);
  stf_size = stuffing(bcc2_aux, 1, aux_array);
  memcpy(packet + res, bcc2_aux, stf_size);
  res += stf_size;

  char flag[BYTE_LEN] = {FLAG_OCT};
  memcpy(packet + res, flag, BYTE_LEN);
  res++;

  llink.frame_size = res;
  memcpy(llink.frame, packet, res);
}

control_t make_Spacket(unsigned char *packet)
{
  unsigned char c = packet[2]; //analise control byte
  if (c == C_SET)
    return SET;
  if (c == C_DISC)
    return DISC;
  if (c == C_UA)
    return UA;
  if (c == C_RR0 || c == C_RR1)
    return RR;
  if (c == C_REJ0 || c == C_REJ1)
    return REJ;
  return CPRT;
}
