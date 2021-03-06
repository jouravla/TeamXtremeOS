#define MAIN

#include "proc.h"
int stringMatch(char *command, char *input, int length);
putChar(char toBePrinted, int row, int col, int color);
putStr(char* strToBePrinted, int row, int col, int color);
int printString(char *str);
int readChar();
int readString(char *buf);
int readSector(char *buf, int absSector);
int mod(int number, int modulo);
int handleInterrupt21(int ax, int bx, int cx, int dx);
int readFile(char *filename, char *buf);
//int executeProgram(char* name, int segment);
void terminate();
int writeSector(char *buffer, int sector);
int deleteFile(char *fname);
int writeFile(char *fname, char *buffer, int sectors);
int findFile(char *fileName, struct directory *dir);
void handleTimerInterrupt(int segment, int stackPointer);
int executeProgram(char* name);
void kStrCopy(char *src, char *dest, int len);
void yield();
void showProcesses();
int kill(int segment);


int main() {
  makeInterrupt21();
  makeTimerInterrupt();
  initializeProcStructures();
  printString("123");
executeProgram("shell\0");

  printString("321");
  while(1);
}

//creates a struct to store name and sectors
struct dirEntry{
  char name[6];
  char sectors[26];
};

//creates a struct to store entries
struct directory {
  struct dirEntry entries[16];
};


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
///////////////////////PROJECT 5/////////////////////////////////////
/*
  Save the stack pointer into the PCB of the running process, mark that process as READY
  Add it to the tail of the ready queue.

  Remove the PCB from the head of the ready queue, mark it as RUNNING, set the running variable to 
  point to it and invoke the returnFromTimer method with the segment and stack pointer of the 
  new running process.  

  If the ready queue happens to be empty then you should complete the above steps using the 
  idle process instead.
*/
void handleTimerInterrupt(int segment, int stackPointer) {
  struct PCB* process;
  setKernelDataSegment();
  running->stackPointer = stackPointer;

 // printString(running->name);

  //terminating programs   
  if(running->state != DEFUNCT && !stringMatch(running->name, "IDLE\0",5)){
    running->state = READY;
    addToReady(running);
  }

  process = removeFromReady();

  //checking to see if ready queue is null
  if(process == NULL){
    running = &idleProc;
  }
  //ready queue is not null
  else{
    process->state = RUNNING;
    running = process;
  }
  returnFromTimer(running->segment, running->stackPointer);
  restoreDataSegment();
}

/*
  Finds an empty memory segment, and then loads the desired program into that segment.
  Then jumps to its first instruction using the launchProgram function.

  Obtains a PCB for the process, initializes the PCB and places it into the ready queue.  
  The PCB should be initialized by setting the name of the process to its filename, 
  the state of the process to STARTING and the segment to the memory segment where the
  process is loaded.  
  The stack pointer should be set to 0xFF00, which will make the top of the process’ 
  stack begin atoffset 0xFF00 within its segment.
*/
int executeProgram(char* name) {
  char buf[13312];
  int i, j, found, segment;
  struct PCB* process;
  char processName[7];

  //return -1 if the program was not found
  found = readFile(name, buf);
  if(found == -1){
    return -1;
  }

  setKernelDataSegment();
  segment = getFreeMemorySegment();
  restoreDataSegment();

  //return -2 if there is no free segment
  if(segment == -1){
    return -2;
  }
  
  //adjusts for the kernel and instruction offset
  segment = 0x1000*(segment+2);
  
  //load the program into that segment
  for(i = 0; i < found * 512; i++) {
    putInMemory(segment, i, buf[i]);
  }
  
  //obtain a free PCB for the process
  setKernelDataSegment();
  process = getFreePCB();
  restoreDataSegment();  

  //if there is no free process
  if(process == NULL){
    printString("There is no free PCB");
    return -2;
  }

  //initializes the PCB
  //name
  kStrCopy(name, process->name, 6);
  setKernelDataSegment();  
  //state
  process->state = STARTING;
  //segment
  process->segment = segment;
  //stack pointer
  process->stackPointer = 0xFF00;

  //adds to ready queue
  addToReady(process);
  restoreDataSegment();

  //initialize program
  initializeProgram(segment);
  return 1;
}

/*
  When a process terminates, you will need to free the memory segment that it is using, 
  free the PCB that it is using, set its state to DEFUNCT and enter an infinite while loop. 
*/
void terminate() {
  setKernelDataSegment();
  releaseMemorySegment((running -> segment)/0x1000 - 2);
  restoreDataSegment();

  setKernelDataSegment();
  releasePCB(running);
  restoreDataSegment();
  
  while(1>0);
}

/* kStrCopy(char *src, char *dest, int len) copy at most len
 * characters from src which is addressed relative to the current 
 * data segment into dest which is addressed relative to the 
 * kernel's data segment (0x1000).
 */
void kStrCopy(char *src, char *dest, int len) {
  int i=0; 
  for (i=0; i<len; i++) {
    putInMemory(0x1000, dest+i, src[i]);
    if (src[i] == 0x00) {
      return;
    }
  }
}

/*
  This function causes the executing process to give up the remainder of 
  its time slice and be put back into the ready queue. 
*/
void yield() {
  setKernelDataSegment();
  addToReady(running);
  restoreDataSegment();
}

/*
  This function should display a list of the names and memory segment indices
  of all of the currently executing processes.
*/
void showProcesses() {
  int i;
    setKernelDataSegment();
     printString("showing");
  
  
  for(i = 0; i < 8; i++){
    if((pcbPool[i].state == RUNNING)){
     printString("showing");
      printString(pcbPool[i].name);
      printString("       ");
      printString((pcbPool[i].segment)/0x1000 - 2);
      printString("\n\r");
    }
  } 
  restoreDataSegment();
}

/*
  This function should kill the process that is executing in the segment with the
  specified index. When the process is killed it no longer executes and its memory
  and PCB are freed.  This function returns 1 if the process is successfully killed
  and -1 if there is no process currently running in the segment with the specified 
  index.  
*/
int kill(int segment) {
  int i;

  segment = (segment+2) *0x1000;
  
  setKernelDataSegment();
  for (i=0; i<8; i++) {
    if(pcbPool[i].segment == segment) {
      pcbPool[i] .state = DEFUNCT;
      releaseMemorySegment(segment);
      releasePCB(&pcbPool[i]);
      restoreDataSegment();
      return 1;
    }
  }
  restoreDataSegment();	
	
  return -1;
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
    return readString(bx);
  }
  //case for readFile
  else if(ax == 0x03) {
    readFile(bx, cx);
  }
  //case for executeProgram
  else if(ax == 0x04) {
    executeProgram(bx);
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
  //case for dir
  else if(ax == 0x98){ 
    readSector(bx,cx);
  }
  //case for yield
  else if(ax == 0x09){ 
    yield();
    return 1;
  }
  //case for showProcesses
  else if(ax == 0x0A){ 
    showProcesses();
    return 1;
  }
  //case for kill
  else if(ax == 0x0B){ 
    return kill(bx);
  }
  else{
    return -1;
  }
}

////////////////////////PROJECT 4////////////////////////////////////

//calculate head, relSector and track. Calculate ax, bx, cx, and dx based off of these
//calculations. Interrupt 0x13 is called with the values for ax, bx, cx and dx we 
//calculated. Return 1 at the end to show sector has been written. 
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

//finds the file passed in to be deleted, if file isn't in Directory, return -1, else
// 0's directory name and sectors, 0's out disk map sectors, then updates directory and map
int deleteFile(char *fname) {
  struct directory *dir;
  char dirBuffer[512];
  char diskMap[512];
  int j = 0;
  int entry = 0;
  readSector(dirBuffer, 2);
  dir = dirBuffer;
  entry = findFile(fname, dir);
  readSector(diskMap, 1);
  
  //if file isn't found
  if(entry == -1){
    return -1;
  }
  else{
    //writes 0 to first entry of file name 
    dir->entries[entry].name[0] = 0x00;
    //marks directory entry sectors as free
    //marks diskMap sectors as free
    while(dir->entries[entry].sectors[j] != 0x00){
      diskMap[dir->entries[entry].sectors[j]] = 0x00; 
      dir->entries[entry].sectors[j] = 0x00;
      j++;
    }  
  }
  //updates the Disk Directory and the Disk Map
  writeSector(dir,2);
  writeSector(diskMap,1);

  return 1;
}


//Helper Method
//returns sector in the directory in which the file is found
int findFile(char *fileName, struct directory *dir){
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

//writes the inputted file via buffer, number of sectors, and buffer
// limits file to 26 sectors, if no entry availible, return -1
// if full file cannot be written due to limited space, return -2
int writeFile(char *fname, char *buffer, int sectors) {
  int i,j,l,m;
  int z = 0;
  int k = 0;
  int writingEntry = 0;
  int unusedSectors[26];
  struct directory *dir;
  char dirBuffer[512];
  char diskMap[512];
  readSector(dirBuffer, 2);
  dir = dirBuffer;
  readSector(diskMap, 1);
  m = 0;

  //makes sure sectors is not larger than 26
  if(sectors > 26) {
    sectors = 26;
  }

  //checks if its a current file
  writingEntry = findFile(fname,dir);
  //if file to be written didn't share name, find empty spot
  if(writingEntry == -1){
    for(i = 0; i < 16; i++){
      if(dir->entries[i].name[0] == 0x00) {
	writingEntry = i;
	break;
      }
    }
  }

  //no available entry slot was found, return -1
  if(writingEntry == -1){
    return -1;
  }

  //find unused sectors in the DiskMap
  for(j = 0; j < 512 && k < sectors; j++){
    if(diskMap[j] == 0x00){
      unusedSectors[k] = j;
      k++;
    }
  }

  //writes inputted name in entry in file directory 
  for(l = 0; l < 6; l++){
    dir->entries[writingEntry].name[l] = fname[l];
  }
  
  
  //writes sectors in DiskMap as taken
  //writes sectors in entry of DiskDirectory as taken
  while(m < sectors){
    diskMap[unusedSectors[m]] = 0xFF;
    dir->entries[writingEntry].sectors[m] = unusedSectors[m];
    m++;
  }

  //write to the sector
  // write buffer/512 portion to writeSector(buffer/512, usedSector[i])
  // removed : dir->entries[writingEntry].sectors[z] !=0
  while(z < k){
    writeSector(&buffer[z*512],dir->entries[writingEntry].sectors[z]);
    z++;
  }
  
  //updates the Disk Directory and the Disk Map
  writeSector(dir,2);
  writeSector(diskMap,1);
 
  //wrote as many sectors as possible, but ran out of room 
  if(z < sectors){
    return -2;
  }
  //else return number of sectors it wrote
  return z;
}

/*
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
  return readString(bx);
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
  else if(ax == 0x98){ 
  readSector(bx,cx);
  }
  else  
  {
  return -1;
  }
  }
*/

///////////////////////////PROJECT 3///////////////////////////////////

//reads in the disk directory from sector and searches it for the indicated fileName. 
//if the fileName is found in the directory then it will read each sector of the file
//and store in the buffer buf.
//else return -1
int readFile(char *fileName, char *buf) {
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

/*
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
*/

//resets the segments and the stack pointer to the memory segment containing the kernel
//prints I'm back! (originally) and executes shell on segment 0x2000. 
/*
  void terminate() {
  resetSegments();
  //printString("I'm back!");
  executeProgram("shell\0",0x2000);
  
  }
*/
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
}*/	
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
  in = readChar();
  while(in != 0x0D) {
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
    in = readChar();
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

