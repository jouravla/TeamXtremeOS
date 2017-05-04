#include "proc.h"

void initializeProcStructures();
int getFreeMemorySegment();
void releaseMemorySegment(int seg);
struct PCB *getFreePCB();
void releasePCB(struct PCB *pcb);
void addToReady(struct PCB *pcb);
struct PCB *removeFromReady();

/*
 * This function initializes all of the global variables and structures defined above. 
 *
 * All entries in the memory map should be marked as FREE.
 *
 * All of the PCBs in the pcbPool should have the first character of their 
 * names set to null, their state set to DEFUNCT and their segment and stack
 * pointers set to 0x0000. 
 *
 * The PCB for the idle process should be initialized with the name IDLE, the state READY, 
 * and both the segment and stack pointers set to 0x0000.  
 *
 * Running should refer to the PCB for the idle process and the ready queue should be empty.
 */  
void initializeProcStructures() {
  int i;

  for(i = 0; i < 8; i++) {
    memoryMap[i] = FREE;

    pcbPool[i].name[0] = 0x00;
    pcbPool[i].state = DEFUNCT;
    pcbPool[i].segment = 0x00;
    pcbPool[i].stackPointer = 0x00;
  }

  idleProc.name[0] = 'I';
  idleProc.name[1] = 'D';
  idleProc.name[2] = 'L';
  idleProc.name[3] = 'E';
  idleProc.name[4] = '\0';

  idleProc.state = READY;
  idleProc.segment = 0x00;
  idleProc.stackPointer = 0x00;

  running = &idleProc;
  
  readyHead = NULL;
  readyTail = NULL;
}

/*
 * Returns the index of the first free memory segment or -1 if 
 * all of the memory segments are used.  The returned memory segment
 * should be marked as used.
 */
int getFreeMemorySegment() {
  int i;

  for(i = 0; i< 8; i++) {
    if(memoryMap[i] == FREE){
      memoryMap[i] = USED;
      return i;
    }
  }
  return -1;
}

/*
 * Release the memory segment indicated by the given index into
 * the memory map.
 */
void releaseMemorySegment(int seg) {
  memoryMap[seg] = FREE;
}

/*
 * Return a pointer to an available PCB from the PCB pool.  All of the
 * available PCBs should be marked as DEFUNCT.  The returned PCB should
 * be marked as STARTING. Return NULL if there are no PCBs available.
 */
struct PCB *getFreePCB() {
  int i;

  for(i = 0; i < 8; i++){
    if(pcbPool[i].state == DEFUNCT){
      pcbPool[i].state = STARTING;
      return &pcbPool[i];
    }
  }
  return NULL;
}

/*
 * Release the provided PCB by setting its state to DEFUNCT, setting
 * its next and prev pointers to NULL and setting the first character
 * of its name to 0x00.
 */
void releasePCB(struct PCB *pcb) {
  pcb->state = DEFUNCT;
  pcb->name[0] = 0x00;

  //if the pcb is at the head
  if(pcb->prev == NULL && pcb -> next != NULL){
    readyHead = pcb->next;
    readyHead->prev = NULL;
  }
  //if the pcb is in the middle
  else if(pcb -> prev != NULL && pcb -> next != NULL){
    pcb -> next -> prev = pcb -> prev;
    pcb -> prev -> next = pcb -> next;
  }
  //if the pcb is at the tail
  else if(pcb->prev != NULL && pcb -> next == NULL){
    readyTail = pcb ->prev;
    readyTail->next = NULL;
  }

  pcb -> next = NULL;
  pcb -> prev = NULL;
}

/*
 * Add the provided PCB to the tail of the ready queue.
 */
void addToReady(struct PCB *pcb) {
  //if the queue is empty
  if(readyHead == NULL){
    readyTail = pcb;
    readyHead = pcb; 
  }
  //if the queue has elements
  else{
    //if there is only one element
    if(readyHead == readyTail){
      readyHead->next = pcb;
      readyTail = pcb;
      readyTail->prev = readyHead;
    }
    //if there is two or more elements
    else{
      pcb->prev = readyTail;
      pcb->prev->next = pcb;
      readyTail = pcb;
    }
  }
}

/*
 * Remove the PCB at the head of the ready queue and return a 
 * pointer to it.
 */
struct PCB *removeFromReady() {
  struct PCB *temp;

  //if there are not any pcbs
  if(readyHead == NULL){
    return NULL;
  }
  //if the queue has one element
  else if(readyHead == readyTail) {
    temp = readyHead;
    readyHead = NULL;
    readyTail = NULL;
  }
  //if the queue has two elements
  else if(readyHead->next == readyTail){
    temp = readyHead;
    readyHead->next->prev = NULL;
    readyHead = readyHead->next;
    readyTail->prev = NULL;
    readyHead->next = NULL;
  }
  //if the queue has more than two elements
  else{
    temp = readyHead;
    readyHead = readyHead->next;
    readyHead->prev = NULL;
  }
  return temp;
}
