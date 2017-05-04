putChar(char toBePrinted, int row, int col, int color);
putStr(char* strToBePrinted, int row, int col, int color);
int printString(char *str);
int readChar();
int readString(char *buf);
int readSector(char *buf, int absSector);
int mod(int number, int modulo);
int handleInterrupt21(int ax, int bx, int cx, int dx);
int readFile(char *filename, char *buf);

int main() {
  char line[80];
  char ch[100];
 
  makeInterrupt21();	// setup ISR for interrupt 0x21

  interrupt(0x21, 0x0, "Type>\0", 0, 0);	// display prompt
  interrupt(0x21, 0x1, ch, 0, 0);			  // read char
  line[0] = ch[0];
  line[1] = 0x00;
  interrupt(0x21, 0x00,ch , 0, 0);			// print string


  while(1>0){}
}

//////////////////////////PROJECT 2 ///////////////////////////////////



//handles all different calls with the interrupt tag of 0x21
//returns 1 if it succeeds
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
  } else {
    return -1;
  }
}
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
