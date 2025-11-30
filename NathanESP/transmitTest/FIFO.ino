#include <stdint.h>


// Declare state variables for FiFo
//        size, buffer, put and get indexes
#define FIFOSIZE 25500

static uint16_t LeftChannelFifo1[FIFOSIZE];
static uint16_t LeftChannelFifo2[FIFOSIZE];
static uint32_t volatile front1;
static uint32_t volatile rear1;
static uint32_t volatile front2;
static uint32_t volatile rear2;

// *********** Fifo1_Init**********
// Initializes a software FIFO1 of a
// fixed size and sets up indexes for
// put and get operations
void LeftChannelFifoInit() {
  rear1 = front1 = 0;
  rear2 = front2 = 0;
}

// *********** Fifo1_Put**********
// Adds an element to the FIFO1
// Input: data is character to be inserted
// Output: 1 for success, data properly saved
//         0 for failure, FIFO1 is full
inline bool LeftChannelFifoPut(bool queue, uint16_t data) {
  if (LeftChannelFifoisFull(queue)) {
    return false;
  }
  else if (queue) {
    LeftChannelFifo1[rear1] = data;
    rear1 = (rear1 + 1) % FIFOSIZE;
    return true;
  }
  else {
    LeftChannelFifo2[rear2] = data;
    rear2 = (rear2 + 1) % FIFOSIZE;
    return true;
  }
}


// *********** Fifo1_Get**********
// Gets an element from the FIFO1
// Input: none
// Output: If the FIFO1 is empty return 0
// If the FIFO1 has data, remove it, and return it
inline bool LeftChannelFifoGet(bool queue, volatile uint16_t* data) {
  if (LeftChannelFifoisEmpty(queue)) {
    return false;
  }
  else if (queue) {
    *data = LeftChannelFifo1[front1];
    front1 = (front1 + 1) % FIFOSIZE;
    return true;
  }
  else {
    *data = LeftChannelFifo2[front2];
    front2 = (front2 + 1) % FIFOSIZE;
    return true;
  }
}

uint16_t LeftChannelFifoCount(bool queue) {
  if (queue) {
    return (rear1 + FIFOSIZE - front1) % FIFOSIZE;
  }
  else {
    return (rear2 + FIFOSIZE - front2) % FIFOSIZE;
  }
}


bool LeftChannelFifoisEmpty(bool queue) {
  if (queue) {
    return rear1 == front1;
  }
  else {
    return rear2 == front2;
  }
}


bool LeftChannelFifoisFull(bool queue) {
  if (queue) {
    return ((rear1 + 1) % FIFOSIZE == front1);
  }
  else {
    return ((rear2 + 1) % FIFOSIZE == front2);
  }
}


