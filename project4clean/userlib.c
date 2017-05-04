#include "userlib.h"

void printString(char *toBePrinted){
  interrupt(0x21, 0x00, toBePrinted, 0, 0);
}

int readChar(){
  interrupt(0x21, 0x11, 0, 0, 0);
}

int readString(char *input){
  interrupt(0x21, 0x01, input, 0, 0);
}

int readFile(char *fileName, char *buffer){
  interrupt(0x21, 0x03, fileName, buffer, 0);
}

void executeFile(char *fileName, int segment){
  interrupt(0x21, 0x04, fileName, segment, 0);
}

void terminate(){
  interrupt(0x21, 0x05, 0, 0, 0);
}


void deleteFile(char *fname){
  interrupt(0x21, 0x07, fname, 0, 0);
}

int writeFile(char *fname, char *buffer, int sectors){
  interrupt(0x21, 0x08, fname, buffer, sectors);
}
int readSector(char *buf, int absSector){
  interrupt(0x21, 0x98, buf, absSector);
}
