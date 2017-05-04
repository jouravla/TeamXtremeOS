#include "userlib.h"

//simple line based text editor that can be executed as a user program
// the editor prompts for input, reads lines of text storing them in a buffer
main() {
  char filename[6];
  char file[13312];
  int tracker = 0;
  int i = 0;
  enableInterrupts();
  printString("Enter filename into which the entered text will be stored: ");
  readString(filename);
  printString("\n\r");
  
  printString("Start typing. Press CTRL+D to break text and Return to save & exit.\n\r");
 
  tracker = readString(file);

   while(file[tracker - 1] != 0x04){
    //printString("\n\r");
    //tracker = readString(file);
    tracker = readString(file+tracker);
  }
   file[tracker-1] =0x00;

   tracker = (tracker+511)/512;
   writeFile(filename, file, tracker);
   terminate();
}

