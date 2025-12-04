hw_timer_t* dacTimer = NULL;
static void IRAM_ATTR soundHandler();
static volatile bool playQueue = false;
static volatile bool refilled = false;
static bool notFastEnough = false;
static portMUX_TYPE receiveMux = portMUX_INITIALIZER_UNLOCKED;

// New var
uint16_t inputdata[10];
bool flangerFlag;
bool pauseFlag;
bool distortionFlag;
bool tremoloFlag;
bool delayFlag;
bool tempoFlag;
#define SCALE_FACTOR 2
//

void setup() {
  Serial.begin(115200);
  delay(2000);
  //Serial.println(">>>> setup start");

  size_t t = Serial2.setRxBufferSize(16384);
  Serial2.begin(300000, SERIAL_8N1, 17, 16); //swap tx and rx because ashston is silly 
  delay(2000);
  //Serial.println("Serial2.begin finished");
  //Serial.printf("Buffer size: %d\n", t);

  // New Code added
  for (int i = 0; i < 10; i++) {
    inputdata[i] = 0;
  }
  flangerFlag = false;
  pauseFlag = false;
  distortionFlag = false;
  tremoloFlag = false;
  delayFlag = false;
  tempoFlag = false;
  //
  
  LeftChannelFifoInit();
  DAC_Init_Left();
  //Serial.println("Waiting for sample rate");
 
  bool waitForSampleRate = true;
  while (waitForSampleRate) {
    uint8_t header;
    if (Serial2.available()) {
      header = Serial2.read();
      Serial.println(header, HEX);
      waitForSampleRate = !(header == 0xC0);
    }
  }

  TimerInit();
}

void TimerInit() {
  //Serial.println("Timer Init");
  while (Serial2.available() < 2) {}
  //Serial.println("Reading sample rate: ");
  uint8_t byte1 = Serial2.read();
  uint8_t byte2 = Serial2.read();
  uint16_t sampleRate = ((byte2 << 8) | byte1) / 3;
  //Serial.println(sampleRate, HEX);

  while (LeftChannelFifoCount(playQueue) < 25499) {
    if (Serial2.available() < 2) {
      Serial2.write('I');
    }
    while (Serial2.available() >= 2) {
      if (!receive(playQueue)) {
        break;
      }
    }
  }
  //Serial.println("Init done");
  //Serial.println(LeftChannelFifoCount(playQueue));

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

static uint16_t lastSample;
void IRAM_ATTR soundHandler() {
  uint16_t data;
  if (!LeftChannelFifoGet(playQueue, &data)) {
    //Serial.println("Empty");
    if (refilled) {
      playQueue = !playQueue;
      refilled = false;
      LeftChannelFifoGet(playQueue, &data);
    }
    else {
      data = lastSample;
      notFastEnough = true;
    }
  } 
  else {
    lastSample = data;
  }
  // if (!LeftChannelFifoGet(queue, &data)) {
  //   data = 2048;
  //   notFastEnough = true;
  // }
  // New Code
  if (UARTRecieve(&inputdata[0])) {
    if (inputdata[9] & 0x1) { // Pause Button
      pauseFlag = !pauseFlag;
    }
    if (inputdata[9] & 0x4) { // Flanger Button
      flangerFlag = !flangerFlag;
    }
    if (inputdata[9] & 0x8) { // Distortion Button
      distortionFlag = !distortionFlag;
    }
    if (inputdata[9] & 0x10) { // Tremolo Button
      tremoloFlag = !tremoloFlag;
    }
    if (inputdata[9] & 0x20) { // Tempo Button
      tempoFlag = !tempoFlag;
    }
    if (inputdata[9] & 0x80) { // Delay Button
      delayFlag = !delayFlag;
    }
  }
  
  if (flangerFlag) {
    uint16_t delay = (inputdata[1] & 0xFFF) / 9;
    uint16_t freq = (inputdata[3] & 0xFFF) / 1024;
    data = flanger_process_left_sample(data, delay, freq);
  }
  if (tempoFlag) {
    // Dont know if we can even implement this at this point. 
  }
  if (pauseFlag) {
    // Pause Stuff Here or Wherever is needed. Just know pauseFlag is set.
  }
  if (delayFlag) {
    uint16_t delaytime = (inputdata[7] & 0xFFF) / 42; 
    data = delay_process_left_sample(data, delaytime);
  }
  if (tremoloFlag) {
    data = tremolo_process_left_sample(data);
  }
  if (distortionFlag) {
    data = distortion_process_left_sample(data);
  }
  //
   DAC_Out_Left(data);
   //DAC_Out_Left(DebugWave[debugSoundIdx << 1]);
   //debugSoundIdx = (debugSoundIdx + 1) % 32;
}

bool receive(bool fillQueue) {
  if (Serial2.available() < 2) {
    return true;
  }
  
  uint8_t byte1 = Serial2.read(); 
  uint8_t byte2 = Serial2.read(); 
  uint16_t sample = (byte2 << 8) | byte1;
  if (sample > 4096) {
    sample = (byte1 << 8) | byte2;
  }
  
  if (!LeftChannelFifoPut(fillQueue, sample)) {
    return false;
  }
  
  return true;
}

void loop() {
  bool fillQueue = !playQueue;

  while (receive(fillQueue));

  refilled = true;
}