#define MAXFRAMESIZE 200
#define START 2
#define END 3

typedef struct
{
    int fileDescriptor; /*Descritor correspondente à porta série*/
    int status;         /*TRANSMITTER | RECEIVER*/
} appLayer;

