#include "userlib.h"
int stringMatch(char *command, char *input, int length);

char line[40];
char buffer[13312];
char name[6];
char name2[6];
int returner = 0;
int numSectors = 0;
int redSector = -1;
char dirBuffer[512];
int j;
struct directory *dir;

//creates a struct to store name and sectors
struct dirEntry{
  char name[6];
  char sectors[26];
};

//creates a struct to store entries
struct directory {
  struct dirEntry entries[16];
};


main() {
  int i = 0;
  while(1) {     
    //print a command prompt (e.g Shell>)
    printString("\n\rShell>");
    //read a line of input from the user
    readString(line);

    //when user enters shell command type check to see if the file exists, if it does
    //then print the contents.
    // removed: line[0] == 't'  && line[1] == 'y' && line[2] == 'p' && line[3] == 'e'
    if(stringMatch(line, "type", 4)) {
      for (i = 0; i < 6; i++){
	name[i] = line[i+5];
      }
      if (readFile(name,buffer) == -1) {
	printString("\n\rFile not found");
      } else {
	printString("\n\r");
	readFile(name,buffer);
	printString(buffer);
      }
    }

    //when user enters shell command execute check to see if the program exists, if
    //it does then execute it
    else if(stringMatch(line, "execute", 7)) {
      for (i = 0; i < 6; i++){
	name[i] = line[i+8];
      }
      printString("\n\r");
      if(executeFile(name, 0x3000) == -1) {
	printString("Program not found");
      }
    }

    //when use enters shell command delete, removes specified file from disk
    // if file doesn't exists, error
    else if(stringMatch(line, "delete",6)) {
      for (i = 0; i < 6; i++){
	name[i] = line[i+7];
      }
      printString("\r");
      if(deleteFile(name) == -1) {
	printString("\nFile not found");
      }
    }

    //when user enters shell command copy, copies the file to new spot if it doesn't share name
    // if the source file not found, error
    // if the directory too fulll, error
    // if the disk becomes too full, error
    else if(stringMatch(line, "copy", 4)) {
      for (i = 0; i < 6; i++){
	name[i] = line[i+5];
      }
 
      for (i = 0; i < 6; i++){
	name2[i] = line[i+12];
      }

      printString("\n\r");
      
      numSectors = readFile(name, buffer);
      
      if(numSectors == -1) {
	printString("File not found");
      }
      
      returner = writeFile(name2, buffer, numSectors);
      if(returner == -2) {
	printString("Disk is full");
      }
      else if(returner == -1){
	printString("Disk Directory is full");
      }
    }

    //when user enters command dir, all filenames are printed 
    else if(stringMatch(line, "dir", 3)) {
      printString("\n\r");
      readSector(dirBuffer, 2);
      dir = dirBuffer;
      for(j = 0; j < 16; j++){
	if(dir->entries[j].name[0] != 0x00){
	  printString(dir->entries[j].name);
	  printString("\n\r");
	}
      }
    }

    else {
      //print "Unrecognized Command."
      printString("\n\rUnrecognized Command");
    }
  }
}

//Helper method
//recognizes commands
int stringMatch(char *command, char *input, int length){
  int i,j;
  for(i = 0; i < length; i++){
    if(command[i] != input[i]){
      return 0;
    }
  }
  return 1;
}
