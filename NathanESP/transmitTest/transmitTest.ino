hw_timer_t* dacTimer = NULL;
static void IRAM_ATTR soundHandler();
static bool queue = false;
static bool nextQueueFull = false;


void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println(">>>> setup start");

  Serial2.begin(2000000, SERIAL_8N1, 16, 17);
  delay(2000);
  Serial.println("Serial2.begin finished");

  
  LeftChannelFifoInit();
  DAC_Init_Left();

  bool waitForSampleRate = true;
  while (waitForSampleRate) {
    uint8_t header;
    if (Serial2.available()) {
      header = Serial2.read();
      waitForSampleRate = !(header == 0xC0);
    }
  }

  TimerInit();
}

bool fillInit() {
  if (Serial2.available() < 4) {
    return false;
  }
  uint8_t byte1 = Serial2.read();
  uint8_t byte2 = Serial2.read();
  uint8_t byte3 = Serial2.read();
  uint8_t byte4 = Serial2.read();

  uint16_t leftSample = (byte2 << 8) | byte1;
  uint16_t rightSample = (byte4 << 8) | byte3;

  //todo error handling
  if (!LeftChannelFifoPut(queue, leftSample)) {
    Serial.println("oops2");
    return false;
  }
  return true;
}

void TimerInit() {
  Serial.println("Timer Init");
  while (Serial2.available() < 2) {}
  Serial.println("Reading sample rate: ");
  uint8_t byte1 = Serial2.read();
  uint8_t byte2 = Serial2.read();
  uint16_t sampleRate = (byte2 << 8) | byte1;
  Serial.println(sampleRate, HEX);

  while (LeftChannelFifoCount(queue) < 25499) {
    if (Serial2.available() < 4) {
      Serial2.write('I');
    }
    while (Serial2.available() >= 4) {
      if (!fillInit()) {
        break;
      }
    }
  }
  Serial.println("Init done");
  Serial.println(LeftChannelFifoCount(queue));

  dacTimer = timerBegin(20000000);
  timerAttachInterrupt(dacTimer, &soundHandler);
  timerAlarm(dacTimer, (20000000 / sampleRate), true, 0);
}

// void changeSampleRate() {
//   uint16_t sampleRate;
//.   get sample rate
//   timerAlarm(dacTimer, (20000000 / sampleRate), true, 0);
// }


static uint16_t debugSoundIdx = 0;
const uint16_t DebugWave[64] = {
  1024, 1122, 1219, 1314, 1407, 1495, 1580, 1658, 1731, 1797, 1855,
  1906, 1948, 1981, 2005, 2019, 2024, 2019, 2005, 1981, 1948, 1906,
  1855, 1797, 1731, 1658, 1580, 1495, 1407, 1314, 1219, 1122, 1024,
  926, 829, 734, 641, 553, 468, 390, 317, 251, 193, 142,
  100, 67, 43, 29, 24, 29, 43, 67, 100, 142, 193,
  251, 317, 390, 468, 553, 641, 734, 829, 926
};


// static void IRAM_ATTR soundHandler() {
//   uint16_t leftData;
//   uint16_t rightData;

//   if (!LeftChannelFifo_Get(&leftData)) {
//     output = false;
//     return;
//   }
//   if (leftData & 0x20) {
//     leftData |= 0xC0;
//   }
//   else {
//     leftData &= 0x3F;
//   }
//   RightChannelFifo_Get(&rightData);
//   DAC_Out_Left(leftData >> 4);
//   output = true;


//   // Debug Sine Wave :
//   // DAC_Out_Left(DebugWave[debugSoundIdx]);
//   // debugSoundIdx = (debugSoundIdx + 1) % 64;
// }

static uint16_t count = 0;
static uint16_t request = 0;

void IRAM_ATTR soundHandler() {
  uint16_t data;
  if (!LeftChannelFifoGet(queue, &data)) {
    //Serial.println("Empty");
    return;
  } else {
    if (data & 0x20) {
      data |= 0xC0;
    } else {
      data &= 0x3F;
    }
  }
  DAC_Out_Left(DebugWave[debugSoundIdx]);
  debugSoundIdx = (debugSoundIdx + 1) % 64;
  
  count++;
  if (count == 25500) {
    if (nextQueueFull) {
      queue = !queue;
      count = 0;
      nextQueueFull = false;
    }
    else {
      Serial.println("Crashing your shit bozo");
    }
  }
}

bool receive() {
  if (Serial2.available() < 4) {
    return false;
  }
  uint8_t byte1 = Serial2.read();
  uint8_t byte2 = Serial2.read();
  uint8_t byte3 = Serial2.read();
  uint8_t byte4 = Serial2.read();

  uint16_t leftSample = (byte2 << 8) | byte1;
  uint16_t rightSample = (byte4 << 8) | byte3;

  //todo error handling
  if (!LeftChannelFifoPut(!queue, leftSample)) {
    Serial.println("oops2");
    return false;
  }
  return true;
}


static uint16_t prevRequest;
static uint16_t nextQueueCount = 0;
static bool serviced = true;

void loop() {
  uint16_t newRequest = request;
  // If we still have more slots to fill and we've already serviced the last request
  if (!nextQueueFull && serviced) {
    Serial2.write('B');
    serviced = false;
  }
  // receive all data
  while (Serial2.available() >= 4 && !nextQueueFull) {
    if (!receive()) {
      break;
    }
    nextQueueCount++;
    serviced = true;
  }
  
  // Check to see if we're full
  if (LeftChannelFifoisFull(!queue)) {
    nextQueueFull = true;
    nextQueueCount = 0;
  }
}