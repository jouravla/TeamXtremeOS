putChar(char toBePrinted, int row, int col, int color);
putStr(char* strToBePrinted, int row, int col, int color);
int printString(char *str);
int readChar();
int readString(char *buf);
int readSector(char *buf, int absSector);
int mod(int number, int modulo);
int handleInterrupt21(int ax, int bx, int cx, int dx);
int readFile(char *filename, char *buf);
int executeProgram(char* name, int segment);
void terminate();
int writeSector(char *buffer, int sector);
int deleteFile(char *fname);
int writeFile(char *fname, char *buffer, int sectors);
int findFile(char *fileName, char *dir);

//creates a struct to store name and sectors
struct dirEntry{
  char name[6];
  char sectors[26];
};

//creates a struct to store entries
struct directory{
  struct dirEntry entries[16];
};

int main() {
  char buf[512];
  buf[0] = "messag";
  deleteFile(buf);
  while(1);
}
////////////////////////PROJECT 4////////////////////////////////////

int writeSector(char *buffer, int sector) {
  char relSector, head, track;
  int ax,bx,cx,dx; 
  relSector = mod(sector, 18) + 1;
  head = mod(sector/18, 2);
  track = (sector/36); 

  ax = 0x03 * 256 + 0x01;
  bx = buffer;
  cx = track * 256 + relSector;
  dx = head * 256 + 0x00;

  interrupt(0x13, ax, bx, cx, dx);
  return 1;

}

int deleteFile(char *fname) {
  struct directory *dir;
  char dirBuffer[512];
  int j = 0;
  int entry;
  readSector(dirBuffer, 2);
  dir = dirBuffer;
  entry = findFile(*fname, *dir);
  
  //if file isn't found
  if(entry == -1){
    return -1;
  }
  else{
    //writes 0 to first entry of file name 
    dir->entries[entry].name[0] = 0x00;
    
    //marks taken sectors as free
    //ask if we should make helper method to find empty sectors not used in any file in the dir
    while(dir->entries[entry].sectors[j] != 0x00){
      dir->entries[entry].sectors[j] == 0x00;
      return 1;
    }  
  }
}

int writeFile(char *fname, char *buffer, int sectors) {
  int i,j,k;
  int check = 0;
  struct directory *dir;
  char dirBuffer[512];
  readSector(dirBuffer, 2);
  dir = dirBuffer;

  //checks if its a current file
  for(i = 0; i < 16; i++){
    if(findFile(*fname, dir) != -1){//not right currently, has to check full name
      check = 1;
      break;
    }
  }
  //if check didn't pass, find empty spot
  if(check == 0){
    for(j = 0; j < 16; j++){
      if(dir->entries[i].name[0] == 0x00)
	break;
    }
  }
  for(k = 0; k < sectors; k++){ 
    //ask if we should make helper method to find empty sectors not used in any file in the dir
    dir->entries[i].sectors[0] == //found empty sector
    //while(buffer
  }
}


int handleInterrupt21(int ax, int bx, int cx, int dx) {
  //case for printString
  if(ax == 0x00) {
    printString(bx);
    return 1;
  }
  //case for readChar()
  else if(ax == 0x11) {
    ((char *) bx)[0] = readChar();
    return 1;
  }
  //case for readString()
  else if(ax == 0x01) {
    readString(bx);
    return 1;
  }
  //case for readFile
  else if(ax == 0x03) {
    readFile(bx, cx);
  }
  //case for executeProgram
  else if(ax == 0x04) {
    executeProgram(bx, cx);
  } 
  //case for terminate
  else if(ax == 0x05) {
    terminate();
  } 
  //case for deleteFile
  else if(ax == 0x07) {
    deleteFile(bx);
  }
  //case for writeFile
  else if(ax == 0x08) {
    writeFile(bx,cx,dx);
  }
  else {
    return -1;
  }
}

//returns sector in the directory in which the file is found
int findFile(char *fileName, char *dir){
  int i, j, z;
  //character matching looking for fileName in the array of entries
  for (i = 0; i < 16; i++) {
    //character match check for filename in the entry
    if(dir->entries[i].name[0] == fileName[0]){
      z=0;
      for(j = 0; j < 6; j++) {
	//if the filename isn't there, mark it not there
	if(dir->entries[i].name[j] != fileName[j]){
	  z=-1;
	}
      }
      if(z==0){
	return i;
      }
    } 
  }
  return -1;
}

///////////////////////////PROJECT 3///////////////////////////////////


//reads in the disk directory from sector and searches it for the indicated fileName. 
//if the fileName is found in the directory then it will read each sector of the file
//and store in the buffer buf.
//else return -1
int readFile(char *fileName, char *buf){
  int i, j, k, z;
  struct directory *dir;
  char dirBuffer[512];
  readSector(dirBuffer, 2);
  dir = dirBuffer;
  //character matching looking for fileName in the array of entries
  for (i = 0; i< 16; i++) {
    //character match check for filename in the entry
    if(dir->entries[i].name[0] == fileName[0]){
      z=0;
      for(j =0; j < 6; j++) {
	if(dir->entries[i].name[j] != fileName[j]){
	  z=1;
	}
      }
      if(z==0){
	//read each sector of the file and store it in buf
	k=0;
	while(dir->entries[i].sectors[k] !=0){
	  readSector(&buf[k*512],dir->entries[i].sectors[k]);
	  k++;
	}
	return k;
      }
    } 
  }
  return -1;
}

//load the program from disk into the buffer. checks to make sure it is in a proper segment
//uses the readFile to load the executable file from disk into the buffer. then transfers
//the executable program into the memory segment where it will be executed. launchProgram
//sets the segment in the stack registers. 
int executeProgram(char* name, int segment) {
  char buf[2000];
  int i;
  int found;

  if(mod(segment, 0x1000) == 0 && segment != 0x0000 && segment != 0x1000 && segment < 0xA000) {
    //return -1 if the program was not found
    found = readFile(name, buf);
    if(found == -1){
      return -1;
    }
   
    //readFile(name, buf);
    for(i = 0; i < found * 512; i++) {
      putInMemory(segment, i, buf[i]);
    }

    launchProgram(segment);
    return 0;
  }
  //return -2 if segment is not valid
  else {
    return -2;
  }
}

//resets the segments and the stack pointer to the memory segment containing the kernel
//prints I'm back! (originally) and executes shell on segment 0x2000. 
void terminate() {
  resetSegments();
  //printString("I'm back!");
  executeProgram("shell\0",0x2000);
  
}

//handles all different calls with the interrupt tag of 0x21
//returns 1 if it succeeds
/*int handleInterrupt21(int ax, int bx, int cx, int dx) {
//case for printString
if(ax == 0x00) {
printString(bx);
return 1;
}
//case for readChar()
else if(ax == 0x11) {
((char *) bx)[0] = readChar();
return 1;
}
//case for readString()
else if(ax == 0x01) {
readString(bx);
return 1;
}
//case for readFile
else if(ax == 0x03) {
readFile(bx, cx);
}
//case for executeProgram
else if(ax == 0x04) {
executeProgram(bx, cx);
} 
//case for terminate
else if(ax == 0x05) {
terminate();
} 
else {
return -1;
}
}	*/
///////////////////PROJECT 2/////////////////////////



//handles all different calls with the interrupt tag of 0x21
//returns 1 if it succeeds
/*int handleInterrupt21(int ax, int bx, int cx, int dx) {
//case for printString
if(ax == 0x00) {
printString(bx);
return 1;
} 
//case for readChar()
else if(ax == 0x11) {
((char *) bx)[0] = readChar();
return 1;
} 
//case for readString()
else if(ax == 0x01) {
readString(bx);
return 1;
} else {
return -1;
}
}*/

//finds the modulo to of a number
//returns the number
int mod(int number, int modulo) {
  int temp = number;
  int div = modulo;
  while(modulo <= number) {
    modulo += div;
    temp -= div;
  }
  return temp;
}
//calculate head, relSector and track. Calculate ax, bx, cx, and dx based off of these
//calculations. Interrupt 0x13 is called with the values for ax, bx, cx and dx we 
//calculated. Return 1 at the end to show sector has been transferred. 
int readSector(char *buf, int absSector) {
  char relSector, head, track;
  int ax,bx,cx,dx; 
  relSector = mod(absSector, 18) + 1;
  head = mod(absSector/18, 2);
  track = (absSector/36); 

  ax = 0x02 * 256 + 0x01;
  bx = buf;
  cx = track * 256 + relSector;
  dx = head * 256 + 0x00;

  interrupt(0x13, ax, bx, cx, dx);
  return 1;

}
//reads the characters into the character array buf until the enter key is pressed
//then adds the null terminator to the end of buf. Each character is echoed as typed,
//and the whole string is echoed after enter is pressed 
//returns the number of characters placed in buf minus the null terminator
//KERNEL IMPROVEMENTS
//if backspace is pressed, the character is erased from buf and from the screen
int readString(char *buf) {
  int i = 0;
  char in = 0x0;
  while(in != 0x0D) {
    in = readChar();
    if(in == 0x08) {
      if(i != 0) {
	buf[i-1] = 0x0;
	i--;
	interrupt(0x10, 0x0E*256 + 0x08, 0, 0, 0);
	interrupt(0x10, 0x0E*256 + 0x20, 0, 0, 0);
	interrupt(0x10, 0x0E*256 + 0x08, 0, 0, 0);
      }
    } else {
      buf[i] = in;
      interrupt(0x10, 0x0E * 256 + in, 0, 0, 0);
      i++;
    }
  }
  buf[i] = 0x00;
  return i;

}
//calls interrupt 0x16 and reads a single character from the keyboard. 
int readChar() {
  char in = 0x0;
  in = interrupt(0x16, 0, 0, 0, 0);
  return (int) in;
}
//using interrupt 0x10, prints the characters of the string until it reaches the
//null terminator and stops. Returns the number of characters printed. 
int printString(char *str) {
  int i;
  int counter = 0;
  char al, ah;
  for(i = 0; str[i] != '\0'; i++) {
    al = str[i];
    ah = 0X0E;
    interrupt(0x10, ah * 256 + al, 0, 0, 0);
    counter++;
  }
  return counter;
}


////////////////////////////////////////Project 1/////////////////////////////////////



//   putChar() accepts the character to be printed, the row and column at which to 
//   print it and the color in which to print it as arguments
putChar(char toBePrinted, int row, int col, int color) {
  //offset takes into account the address relative to the beginning of video memory
  int offset = (80*(row - 1))*2 + (col*2);

  putInMemory(0xB800, offset, toBePrinted);
  putInMemory(0xB800, offset + 1, color);
}

//   putStr() accepts the character to be printed, the row and column at which to 
//   print the first character of the string and the color in which to print it 
//   as arguments.  Each successive character in the string should be printed one 
//   column to the right of the previous one

putStr(char* strToBePrinted, int row, int col, int color) {
  int i;
  int currentRow = row;
  int currentCol = col;

  //loops through the entirety of the string until the endline character is hit
  for(i=0; strToBePrinted[i] != '\0'; i++){
    //if col # > 80, reset to 0 and add 1 to row
    if(currentCol > 80){
      currentRow ++;
      currentCol = 1;
    }
    
    //calls putChar to finish printing and increments currentCol for the next column 
    putChar(strToBePrinted[i], currentRow, currentCol, color);
    currentCol++;
  }
}

