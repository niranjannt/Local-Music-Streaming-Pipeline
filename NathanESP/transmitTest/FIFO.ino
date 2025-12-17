#include <stdint.h>

#define FIFOSIZE 25500
static uint16_t Fifo1[FIFOSIZE];
static uint16_t Fifo2[FIFOSIZE];
static uint32_t volatile front1;
static uint32_t volatile rear1;
static uint32_t volatile front2;
static uint32_t volatile rear2;


void FifoInit() {
  rear1 = front1 = 0;
  rear2 = front2 = 0;
}

inline bool FifoPut(bool queue, uint16_t data) {
  if (queue) {
    if ((rear1 + 1) % FIFOSIZE == front1) {
      return false;
    }
    Fifo1[rear1] = data;
    rear1 = (rear1 + 1) % FIFOSIZE;
    return true;
  }
  else {
    if ((rear2 + 1) % FIFOSIZE == front2) {
      return false;
    }
    Fifo2[rear2] = data;
    rear2 = (rear2 + 1) % FIFOSIZE;
    return true;
  }
}


inline bool FifoGet(bool queue, volatile uint16_t* data) {
  if (queue) {
    if (rear1 == front1) {
      return false;
    }
    *data = Fifo1[front1];
    front1 = (front1 + 1) % FIFOSIZE;
    return true;
  }
  else {
    if (rear2 == front2) {
      return false;
    }
    *data = Fifo2[front2];
    front2 = (front2 + 1) % FIFOSIZE;
    return true;
  }
}

uint16_t FifoCount(bool queue) {
  if (queue) {
    return (rear1 + FIFOSIZE - front1) % FIFOSIZE;
  }
  else {
    return (rear2 + FIFOSIZE - front2) % FIFOSIZE;
  }
}


bool FifoisEmpty(bool queue) {
  if (queue) {
    return rear1 == front1;
  }
  else {
    return rear2 == front2;
  }
}


inline bool FifoisFull(bool queue) {
  if (queue) {
    return ((rear1 + 1) % FIFOSIZE == front1);
  }
  else {
    return ((rear2 + 1) % FIFOSIZE == front2);
  }
}