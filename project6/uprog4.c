main(){

  interrupt(0x21, 0x00, "Marshall smells\n\r\0", 0, 0);
  sleep(2);
  interrupt(0x21,0x00,"Sasha is useless\n\r\0",0,0);
  interrupt(0x21, 0x05, 0, 0, 0);
}

