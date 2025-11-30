hw_timer_t* dacTimer = NULL;
static void IRAM_ATTR soundHandler();
static bool queue = false;
static bool nextQueueFull = false;
static bool notFastEnough = false;


void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println(">>>> setup start");

  Serial2.begin(3000000, SERIAL_8N1, 16, 17);
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
  //uint8_t byte3 = Serial2.read();
  //uint8_t byte4 = Serial2.read();

  uint16_t leftSample = (byte2 << 8) | byte1;
  //uint16_t rightSample = (byte4 << 8) | byte3;

  //todo error handling
  if (!LeftChannelFifoPut(queue, leftSample)) {
    Serial.println('1');
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
  uint16_t sampleRate = ((byte2 << 8) | byte1) >> 1;
  Serial.println(sampleRate, HEX);

  while (LeftChannelFifoCount(queue) < 25499) {
    if (Serial2.available() < 2) {
      Serial2.write('I');
    }
    while (Serial2.available() >= 2) {
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
//   get sample rate
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

void IRAM_ATTR soundHandler() {
  uint16_t data;
  if (!LeftChannelFifoGet(queue, &data)) {
    //Serial.println("Empty");
    if (nextQueueFull) {
      queue = !queue;
      nextQueueFull = false;
    }
    else {
      notFastEnough = true;
    };
  } 
  // else {
  //   if (data & 0x20) {
  //     data |= 0xC0;
  //   } else {
  //     data &= 0x3F;
  //   }
  // }
  // data = data >> 4;

  DAC_Out_Left(DebugWave[debugSoundIdx << 1]);
  debugSoundIdx = (debugSoundIdx + 1) % 32;
}

bool receive() {
  if (Serial.available() < 2) {
    return true;
  }
  uint8_t byte1 = Serial2.read();
  uint8_t byte2 = Serial2.read();
  //uint8_t byte3 = Serial2.read();
  //uint8_t byte4 = Serial2.read();

  //uint16_t leftSample = (byte2 << 8) | byte1;
  //uint16_t rightSample = (byte4 << 8) | byte3;

  //todo error handling
  if (!LeftChannelFifoPut(!queue, ((byte2 << 8) | byte1))) {
    //Serial.println('2');
    return false;
  }
  return true;
}


static uint16_t prevRequest;
static uint16_t nextQueueCount = 0;
static bool serviced = true;

void loop() {
  // If we still have more slots to fill and we've already serviced the last request
  if (!nextQueueFull && !nextQueueFull) {
    Serial2.write('B');
    while (receive()) {}
    nextQueueFull = true;
  }
  // receive all data
  // while (Serial.available() >= 2 && !nextQueueFull) {
  //   if (!receive() && LeftChannelFifoisFull(!queue)) {
  //     nextQueueFull = true;
  //     break;
  //   }
  //   serviced = true;
  // }
  // while (!nextQueueFull) {
  //   nextQueueFull = receive();
  // }
  if (notFastEnough) {
    Serial.println("failed");
  }
}