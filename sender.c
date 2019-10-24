/*WRITE Non-Canonical Input Processing*/
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include "llmacros.h"
#include "interface.h"
#include "applicationLayer.h"

int main(int argc, char **argv)
{
  if ((argc < 2) || (atoi(argv[1]) < 0 || atoi(argv[1]) > 2))
  {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttySx\n");
    exit(1);
  }

  //perguntar ao utilizador o nome do ficheiro a enviar
  char file_name[13] = "textfile.txt";

  applicationLayerSender(atoi(argv[1]), file_name);

  sleep(1);

  return 0;
}
