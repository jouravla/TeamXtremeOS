/*
 *  testproc.c
 *  
 *
 *  Created by Grant Braught on 4/1/10.
 *  Copyright 2010 Dickinson College. All rights reserved.
 *
 */

#define MAIN

#include "stdio.h"
#include "assert.h"
#include <string.h>
#include "proc.h"

void testInit() {	
  initializeProcStructures();
	
  assert(running == &idleProc);
  assert(readyHead == NULL);
  assert(readyTail == NULL);
  int i=0;
  for (i=0; i<8; i++) {
    assert(memoryMap[i] == FREE);
    assert(pcbPool[i].name[0] == 0x00);
    assert(pcbPool[i].state == DEFUNCT);
    assert(pcbPool[i].segment == 0x00);
    assert(pcbPool[i].stackPointer == 0x00);
  }
	
  assert(strcmp(idleProc.name, "IDLE\0") == 0);
  assert(idleProc.segment == 0x0000);
  assert(idleProc.stackPointer == 0x0000); 
}


void testGetFreeMemorySegment(){
  initializeProcStructures();
	
  int i, check;
  //checks that the 8 free segments are being properly gotten
  for (i = 0; i < 8; i++) {
    check = getFreeMemorySegment();
    assert(check == i);
    assert(memoryMap[i] == USED);
  }
  
  //checks that there are no free segments left
  check = getFreeMemorySegment();
  assert(check == -1);
}


void testReleaseMemorySegment(){
  initializeProcStructures();

  //makes sure every segment is mapped to USED
  testGetFreeMemorySegment();
	
  int i;
  //makes sure that they're all mapped to FREE
  for (i = 0; i < 8; i++) {
    releaseMemorySegment(i);
    assert(memoryMap[i] == FREE);
  }
}


void testGetFreePCB(){
  initializeProcStructures();

  int i;
  struct PCB *check;

  //check that the method returns a pointer to the available PCB 
  //and that they're marked as STARTING
  for(i = 0; i < 8; i++){
    check = getFreePCB();
    assert(pcbPool[i].state = STARTING);
    assert(check == &pcbPool[i]);
  }
  
  //check that there are no free PCBs left
  check = getFreePCB();
  assert(check == NULL);
}

void testReleasePCB(){
  initializeProcStructures();

  int i;
  struct PCB *check[8];

  for(i = 0; i < 8; i++){
    check[i] = getFreePCB();
  }
  for(i = 0; i < 8; i++){
    releasePCB(check[i]);//segmentation fault here 
    assert(pcbPool[i].state == DEFUNCT);
    assert(pcbPool[i].name[0] == 0x00);
    assert(pcbPool[i].next == NULL);
    assert(pcbPool[i].prev == NULL);
  }
}


void testAddToReady(){
  initializeProcStructures();

  struct PCB check1;
  struct PCB check2;
  struct PCB check3;

  //check that adding a PCB to the an empty queue works
  addToReady(&check1);
  assert(readyHead == &check1);
  assert(readyTail == &check1);

  //check that adding a PCB to a queue with only one element works
  addToReady(&check2);
  assert(readyHead == &check1);
  assert(readyHead->next == &check2);
  assert(readyTail == &check2);
  assert(readyTail->prev == &check1);

  //check that adding a PCB to a queue with more than two elements works
  addToReady(&check3);
  assert(readyHead == &check1);
  assert(readyHead->next == &check2);
  assert(readyHead->next->next == &check3);
  assert(readyTail == &check3);
  assert(readyTail->prev == &check2);
  assert(readyTail->prev->prev == &check1);
}


void testRemoveFromReady(){
  initializeProcStructures();

  struct PCB check1;
  struct PCB check2;
  struct PCB check3;
  struct PCB *check4;
  //check that removing a PCB from an empty queue returns NULL
  check4 = removeFromReady();
  assert(check4 == NULL);

  //check that removing one PCB from a queue of 1 empties queue
  addToReady(&check1);
  check4 = removeFromReady();
  assert(check4 == &check1);
  assert(readyHead == NULL);
  assert(readyTail == NULL);

  //check that removing a PCB from a queue of 2, reduces it to a queue of 1
  addToReady(&check1);
  addToReady(&check2);
  check4 = removeFromReady();
  assert(check4 == &check1);
  assert(readyHead == &check2);
  assert(readyTail == &check2);
  assert(readyHead->next == NULL);
  assert(readyHead->prev == NULL);

  //empties queue
  check4 = removeFromReady();
  check4 = NULL;
  //check that removing from a queue of 3 reduces to a queue of 1
  addToReady(&check1);
  addToReady(&check2);
  addToReady(&check3);
  check4 = removeFromReady();
  assert(check4 == &check1);
  assert(readyHead == &check2);
  assert(readyTail == &check3);
  assert(readyHead->next == &check3);
  assert(readyTail->prev == &check2);
  assert(readyHead->prev == NULL);
}

int main() {
  printf("Testing initializeProcStructures\n");
  testInit();

  printf("Testing getFreeMemorySegment\n");
  testGetFreeMemorySegment();

  printf("Testing releaseMemorySegment\n");
  testReleaseMemorySegment();

  printf("Testing getFreePCB\n");
  testGetFreePCB();

  printf("Testing releasePCB\n");
  testReleasePCB();

  printf("Testing addToReady\n");
  testAddToReady();

  printf("Testing removeFromReady\n");
  testRemoveFromReady();

  printf("done\n");	
}
