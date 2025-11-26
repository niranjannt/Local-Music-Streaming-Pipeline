#include <stdint.h>


// Declare state variables for FiFo
//        size, buffer, put and get indexes
#define FIFOSIZE 4096
uint16_t static RightChannelFifo[FIFOSIZE];
uint32_t static FifoGetRight;
uint32_t static FifoPutRight;

// *********** Fifo1_Init**********
// Initializes a software FIFO1 of a
// fixed size and sets up indexes for
// put and get operations
void RightChannelFifo_Init() {
  FifoGetRight = FifoPutRight = 0;
}

// *********** Fifo1_Put**********
// Adds an element to the FIFO1
// Input: data is character to be inserted
// Output: 1 for success, data properly saved
//         0 for failure, FIFO1 is full
bool RightChannelFifo_Put(uint16_t data) {
//Complete this routine
  if( ((FifoPutRight + 1) & (FIFOSIZE - 1)) == FifoGetRight) {
      return false;
  }
  RightChannelFifo[FifoPutRight] = data;
  FifoPutRight = (FifoPutRight + 1) & (FIFOSIZE - 1);
  return true;
}



// *********** Fifo1_Get**********
// Gets an element from the FIFO1
// Input: none
// Output: If the FIFO1 is empty return 0
//         If the FIFO1 has data, remove it, and return it
bool RightChannelFifo_Get(uint16_t* data) {
  if (FifoGetRight==FifoPutRight) {
      return false;
  }
  *data = RightChannelFifo[FifoGetRight];
  FifoGetRight = (FifoGetRight + 1) & (FIFOSIZE - 1);
}


uint16_t static LeftChannelFifo[FIFOSIZE];
uint32_t static FifoGetLeft;
uint32_t static FifoPutLeft;

// *********** Fifo1_Init**********
// Initializes a software FIFO1 of a
// fixed size and sets up indexes for
// put and get operations
void LeftChannelFifo_Init() {
  FifoGetLeft = FifoPutLeft = 0;
}

// *********** Fifo1_Put**********
// Adds an element to the FIFO1
// Input: data is character to be inserted
// Output: 1 for success, data properly saved
//         0 for failure, FIFO1 is full
bool LeftChannelFifo_Put(uint16_t data) {
  if (((FifoPutLeft + 1) & (FIFOSIZE - 1)) == FifoGetLeft) {
    return false;
  }
  LeftChannelFifo[FifoPutLeft] = data;
  FifoPutLeft = (FifoPutLeft + 1) & (FIFOSIZE - 1);
  return true;
}




// *********** Fifo1_Get**********
// Gets an element from the FIFO1
// Input: none
// Output: If the FIFO1 is empty return 0
//         If the FIFO1 has data, remove it, and return it
bool LeftChannelFifo_Get(uint16_t* data) {
  if (FifoGetLeft == FifoPutLeft) {
    return false;
  }
  *data = LeftChannelFifo[FifoGetLeft];
  FifoGetLeft = (FifoGetLeft + 1) & (FIFOSIZE - 1);
  return true;
}


bool LeftChannelisEmpty(void) {
  return FifoGetLeft == FifoPutLeft;
}


bool RightChannelisEmpty(void) {
  return FifoGetRight == FifoPutRight;
}

bool LeftChannelisFull(void){
  return ((FifoPutLeft + 1) & (FIFOSIZE - 1)) == FifoGetLeft;
}

uint16_t RightChannelisFull(void){
  return ((FifoPutRight + 1) & (FIFOSIZE - 1)) == FifoGetRight;
}



