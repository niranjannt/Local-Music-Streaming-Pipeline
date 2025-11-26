
#include <stdint.h>


// Declare state variables for FiFo
//        size, buffer, put and get indexes
#define FIFOSIZE 4096
uint16_t static RightChannelFifo[FIFOSIZE];
uint32_t static FIFOgetRight;
uint32_t static FIFOputRight;

// *********** Fifo1_Init**********
// Initializes a software FIFO1 of a
// fixed size and sets up indexes for
// put and get operations
void RightChannelFifo_Init() {
  FIFOgetRight=FIFOputRight=0;
}

// *********** Fifo1_Put**********
// Adds an element to the FIFO1
// Input: data is character to be inserted
// Output: 1 for success, data properly saved
//         0 for failure, FIFO1 is full
bool RightChannelFifo_Put(uint16_t data) {
//Complete this routine
  if( ((FIFOputRight + 1) & (FIFOSIZE - 1)) == FIFOgetRight) {
      return false;
  }
  RightChannelFifo[FIFOputRight] = data;
  FIFOputRight = (FIFOputRight + 1) & (FIFOSIZE - 1);
  return true;
}



// *********** Fifo1_Get**********
// Gets an element from the FIFO1
// Input: none
// Output: If the FIFO1 is empty return 0
//         If the FIFO1 has data, remove it, and return it
bool RightChannelFifo_Get(uint16_t* data) {
  if (FIFOgetRight==FIFOputRight) {
      return false;
  }
  *data = RightChannelFifo[FIFOgetRight];
  FIFOgetRight = (FIFOgetRight + 1) & (FIFOSIZE - 1);
}


uint16_t static LeftChannelFifo[FIFOSIZE];
uint32_t static FIFOgetLeft;
uint32_t static FIFOputLeft;

// *********** Fifo1_Init**********
// Initializes a software FIFO1 of a
// fixed size and sets up indexes for
// put and get operations
void LeftChannelFifo_Init() {
  FIFOgetLeft=FIFOputLeft=0;
}

// *********** Fifo1_Put**********
// Adds an element to the FIFO1
// Input: data is character to be inserted
// Output: 1 for success, data properly saved
//         0 for failure, FIFO1 is full
bool LeftChannelFifo_Put(uint16_t data) {
  if (((FIFOputLeft + 1) & (FIFOSIZE - 1)) == FIFOgetLeft) {
    return false;
  }
  LeftChannelFifo[FIFOputLeft] = data;
  FIFOputLeft = (FIFOputLeft + 1) & (FIFOSIZE - 1);
  return true;
}




// *********** Fifo1_Get**********
// Gets an element from the FIFO1
// Input: none
// Output: If the FIFO1 is empty return 0
//         If the FIFO1 has data, remove it, and return it
bool LeftChannelFifo_Get(uint16_t* data) {
  if (FIFOgetLeft == FIFOputLeft) {
    return false;
  }
  data = LeftChannelFifo[FIFOgetLeft];
  FIFOgetLeft = (FIFOgetLeft + 1) & (FIFOSIZE - 1);
  return true;
}


bool LeftChannelisEmpty(void) {
  return FIFOgetLeft == FIFOputLeft;
}


}

bool RightChannelisEmpty(void) {
  return FIFOgetRight == FIFOputRight;
}

bool LeftChannelisFull(void){
  return ((FIFOputLeft + 1) & (FIFOSIZE - 1)) == FIFOgetLeft;
}

uint16_t RightChannelisFull(void){
  return ((FIFOputRight + 1) & (FIFOSIZE - 1)) == FIFOgetRight;
}



