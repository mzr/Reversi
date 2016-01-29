#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "pipes.h"

struct pipes {
    FILE *fifo_in, *fifo_out;
    int isC;
} ;

int fileno(FILE *file);
static FILE *openOutPipe(char *name);
static FILE *openInPipe(char *name);

void closePipes(PipesPtr pipes)
{
  fclose(pipes->fifo_in);
  fclose(pipes->fifo_out);
  free(pipes);
  remove("BtoC");
  remove("CtoB");
}

PipesPtr initPipes(int argc,char *argv[])
{
    if (argc < 2 || (argv[1][0] != 'C' && argv[1][0] != 'B') || argv[1][1] != '\0')
    {
    fprintf(stderr,"\nThis program should be called with the first argument: C or B\n\n");
	mkfifo("AtoB",0664);
	mkfifo("BtoA",0664);
	fprintf(stderr,"Fifo queues CtoB and BtoC created\n");
        return NULL;
    }
    PipesPtr pipes=(PipesPtr)malloc(sizeof(struct pipes));
    if (pipes == NULL) {
        fprintf(stderr,"Memory allocation error\n");
    } else {
        pipes->isC=(argv[1][0] == 'C');
        pipes->fifo_out = openOutPipe(pipes->isC ? "CtoB" : "BtoC");
        pipes->fifo_in = openInPipe(pipes->isC ? "BtoC" : "CtoB");
    }
    return pipes;
}

static FILE *openOutPipe(char *name) {
    FILE *pipe = fopen(name, "w+");
    if (pipe == NULL)
    {
        fprintf(stderr,"Error in creating output pipe");
        exit(-1);
    }
    return pipe;
}

static FILE *openInPipe(char *name){
    FILE *pipe = fopen(name, "r+");
    if (pipe == NULL)
    {
        fprintf(stderr,"Error in creating input pipe");
        exit(-1);
    }
    int flags, fd;
    fd = fileno(pipe);
    flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return pipe;
}

void sendStringToPipe(PipesPtr pipes, const char *data)
{
    int result = fprintf(pipes->fifo_out,"%s",data);
    fflush(pipes->fifo_out);
    if (result == 0)
        fprintf(stderr,"Failed to send data");
}

bool getStringFromPipe(PipesPtr pipes, char *buffer, size_t size)
{
    char *result = fgets(buffer,size,pipes->fifo_in);
    fflush(pipes->fifo_in);
    //if (result == NULL) fprintf(stderr,"Failed to read data");
    return result != NULL;
}
