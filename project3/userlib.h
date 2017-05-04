#ifndef USER_LIB
#define USER_LIB

void printString(char *toBePrinted);
void readChar();
void readString(char *input);
void readFile(char *fileName, char *buffer);
void executeFile(char *fileName, int segment);
void terminate();

#endif

