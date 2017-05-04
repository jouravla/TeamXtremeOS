putChar(char toBePrinted, int row, int col, int color);
putStr(char* strToBePrinted, int row, int col, int color);

int main() {
  //A Hello World Kernel
  putInMemory(0xB800, 0x000, 'H');
  putInMemory(0xB800, 0x001, 0x0F);
  putInMemory(0xB800, 0x002, 'e');
  putInMemory(0xB800, 0x003, 0x0F);
  putInMemory(0xB800, 0x004, 'l');
  putInMemory(0xB800, 0x005, 0x0F);    
  putInMemory(0xB800, 0x006, 'l');
  putInMemory(0xB800, 0x007, 0x0F);
  putInMemory(0xB800, 0x008, 'o');
  putInMemory(0xB800, 0x009, 0x0F);
  putInMemory(0xB800, 0x00A, ' ');
  putInMemory(0xB800, 0x00C, 'W');
  putInMemory(0xB800, 0x00D, 0x0F);
  putInMemory(0xB800, 0x00E, 'o');
  putInMemory(0xB800, 0x00F, 0x0F);
  putInMemory(0xB800, 0x010, 'r');
  putInMemory(0xB800, 0x011, 0x0F);
  putInMemory(0xB800, 0x012, 'l');
  putInMemory(0xB800, 0x013, 0x0F);
  putInMemory(0xB800, 0x014, 'd');
  putInMemory(0xB800, 0x015, 0x0F);

  //Kernel Improvments #1
  putChar('H', 13, 35, 0x4F);
  putChar('e', 13, 36, 0x4F);
  putChar('l', 13, 37, 0x4F);
  putChar('l', 13, 38, 0x4F);
  putChar('o', 13, 39, 0x4F);
  putChar(' ', 13, 40, 0x4F);
  putChar('W', 13, 41, 0x4F);
  putChar('o', 13, 42, 0x4F);
  putChar('r', 13, 43, 0x4F);
  putChar('l', 13, 44, 0x4F);
  putChar('d', 13, 45, 0x4F);

  //Kernel Improvements #2
  putStr("Hello World", 25, 69, 0xF4);
	while(1);


}

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
