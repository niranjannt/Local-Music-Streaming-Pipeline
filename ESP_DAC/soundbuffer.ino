// FIFO1.c
// Runs on any microcontroller
// Provide functions that implement the Software FiFo Buffer
// Last Modified: 10/29/2023
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

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
void RightChannelFifo_Init(){
//Complete this
    FIFOgetRight=FIFOputRight=0;

}

// *********** Fifo1_Put**********
// Adds an element to the FIFO1
// Input: data is character to be inserted
// Output: 1 for success, data properly saved
//         0 for failure, FIFO1 is full
uint32_t RightChannelFifo_Put(uint16_t data){
  //Complete this routine
    if( ((FIFOputRight+1)&(FIFOSIZE-1))==FIFOgetRight){
        return 0;
    }
    RightChannelFifo[FIFOputRight]=data;
    FIFOputRight=(FIFOputRight+1)&(FIFOSIZE-1);
    return 1;
    }




// *********** Fifo1_Get**********
// Gets an element from the FIFO1
// Input: none
// Output: If the FIFO1 is empty return 0
//         If the FIFO1 has data, remove it, and return it
uint16_t RightChannelFifo_Get(void){
uint16_t data=0;



if(FIFOgetRight==FIFOputRight){
    return 0;
}

// replace this line with your solution
data=RightChannelFifo[FIFOgetRight];
FIFOgetRight= (FIFOgetRight+1)&(FIFOSIZE-1);
return data;
}


uint16_t static LeftChannelFifo[FIFOSIZE];
uint32_t static FIFOgetLeft;
uint32_t static FIFOputLeft;

// *********** Fifo1_Init**********
// Initializes a software FIFO1 of a
// fixed size and sets up indexes for
// put and get operations
void LeftChannelFifo_Init(){
//Complete this
    FIFOgetLeft=FIFOputLeft=0;

}

// *********** Fifo1_Put**********
// Adds an element to the FIFO1
// Input: data is character to be inserted
// Output: 1 for success, data properly saved
//         0 for failure, FIFO1 is full
uint32_t LeftChannelFifo_Put(uint16_t data){
  //Complete this routine
    if( ((FIFOputLeft+1)&(FIFOSIZE-1))==FIFOgetLeft){
        return 0;
    }
    LeftChannelFifo[FIFOputLeft]=data;
    FIFOputLeft=(FIFOputLeft+1)&(FIFOSIZE-1);
    return 1;
    }




// *********** Fifo1_Get**********
// Gets an element from the FIFO1
// Input: none
// Output: If the FIFO1 is empty return 0
//         If the FIFO1 has data, remove it, and return it
uint16_t LeftChannelFifo_Get(void){
uint16_t data=0;



if(FIFOgetLeft==FIFOputLeft){
    return 0;
}

// replace this line with your solution
data=LeftChannelFifo[FIFOgetLeft];
FIFOgetLeft= (FIFOgetLeft+1)&(FIFOSIZE-1);
return data;
}


uint16_t LeftChannelisEmpty(void){
if(FIFOgetLeft==FIFOputLeft){
    return 1;
}

else{

  return 0;
}


}

uint16_t RightChannelisEmpty(void){
if(FIFOgetRight==FIFOputRight){
    return 1;
}

else{

  return 0;
}


}


uint16_t LeftChannelisFull(void){
    if( ((FIFOputLeft+1)&(FIFOSIZE-1))==FIFOgetLeft){
        return 1;
    }


else{

  return 0;
}


}

uint16_t RightChannelisFull(void){
  if( ((FIFOputRight+1)&(FIFOSIZE-1))==FIFOgetRight){
        return 1;
    }

else{

  return 0;
}


}



