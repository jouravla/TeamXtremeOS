#include "userlib.h" 
char line[40];
char buffer[40];
char name[6];
char temp[15];
main() {
  int i = 0;
  while(1) {     
    
    //print a command prompt (e.g Shell>)
    printString("\n\rShell>");
    //read a line of input from the user
    readString(line);
    //when user enters shell command type check to see if the file exists, if it does
    //then print the contents.
    if(line[0] == 't'  && line[1] == 'y' && line[2] == 'p' && line[3] == 'e') {
      for (i = 0; i < 6; i++){
	name[i] = line[i+5];
      }
      if (readFile(name,buffer) == -1 || line[12] != '\0') {
	printString("\n\rFile not found");
      } else {
	printString("\n");
	readFile(name,buffer);
	printString(buffer);
      }
    }
    //when user enters shell command execute check to see if the program exists, if
    //it does then execute it
    else if(line[0] == 'e' && line[1] == 'x' && line[2] == 'e' && line[3] == 'c' &&
	    line[4] == 'u' && line[5] == 't' && line[6] == 'e') {
      for (i = 0; i < 6; i++){
	name[i] = line[i+8];
      }
      printString("\n\r");
      if(executeFile(name, 0x3000) == -1) {
	printString("Program not found");
      }
    }
    else {
      //print "Unrecognized Command."
      printString("\n\rUnrecognized Command");
    }
  }

}
