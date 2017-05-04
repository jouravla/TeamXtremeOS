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

void executeFile(char *fileName){
  interrupt(0x21, 0x04, fileName,0, 0);
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



void yield(){
  interrupt(0x21, 0x09, 0, 0, 0);
}

void showProcesses(){
  interrupt(0x21, 0x0A, 0, 0, 0);
}

int kill(int segment){
  interrupt(0x21, 0x0B, segment, 0, 0);
}

void sleep(int seconds) {
  interrupt(0x21,0xA1, seconds, 0, 0);
} 
