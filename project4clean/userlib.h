#ifndef USER_LIB
#define USER_LIB

void printString(char *toBePrinted);
int readChar();
int readString(char *input);
int readFile(char *fileName, char *buffer);
void executeFile(char *fileName, int segment);
void terminate();

void deleteFile(char *fname);
int writeFile(char *fname, char *buffer, int sectors);
int readSector(char *buf, int absSector);

#endif

