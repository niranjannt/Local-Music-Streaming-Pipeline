hw_timer_t* dacTimer = NULL;
static void IRAM_ATTR soundHandler();
static volatile bool output = false;
void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println(">>>> setup start");

  Serial2.begin(2000000, SERIAL_8N1, 16, 17);
  delay(2000);
  Serial.println("Serial2.begin finished");

  RightChannelFifo_Init();
  LeftChannelFifo_Init();
  DAC_Init_Left();
  DAC_Init_Right();

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

void TimerInit() {
  Serial.println("Timer Init");
  while (Serial2.available() < 2) {}
  Serial.println("Reading sample rate: ");
  uint8_t byte1 = Serial2.read();
  uint8_t byte2 = Serial2.read();
  uint16_t sampleRate = (byte2 << 8) | byte1;
  Serial.println(sampleRate, HEX);

  const int PREFILL_FRAMES = 4096;
  int got = 0;
  unsigned long t0 = millis();
  while (got < PREFILL_FRAMES && millis() - t0 < 5000) {
    if (Serial2.available() < 4) {
      Serial2.write('R');    
      delayMicroseconds(200); 
    }
    while (Serial2.available() >= 4) {
      if (receive()) {
        got++;
      } 
      else {
        break;
      }
    }
  }
  Serial.println("Init done");
  Serial.println(LeftChannelCount());

  dacTimer = timerBegin(40000000);
  timerAttachInterrupt(dacTimer, &soundHandler);
  timerAlarm(dacTimer, (40000000 / sampleRate), true, 0); 
}

void changeSampleRate() {
  uint16_t sampleRate; 
  RightChannelFifo_Get(&sampleRate);
  timerAlarm(dacTimer, (40000000 / sampleRate), true, 0);
}


static uint16_t DebugSoundIdx = 0;
const uint16_t DebugWave[64] = {  
  1024,1122,1219,1314,1407,1495,1580,1658,1731,1797,1855,
  1906,1948,1981,2005,2019,2024,2019,2005,1981,1948,1906,
  1855,1797,1731,1658,1580,1495,1407,1314,1219,1122,1024,
  926,829,734,641,553,468,390,317,251,193,142,
  100,67,43,29,24,29,43,67,100,142,193,
  251,317,390,468,553,641,734,829,926
};  


static void IRAM_ATTR soundHandler() {
  uint16_t leftData;
  uint16_t rightData;

  if (!LeftChannelFifo_Get(&leftData)) {
    output = false;
    return;
  }
  RightChannelFifo_Get(&rightData);
  DAC_Out_Left(leftData);
  output = true;


  // Debug Sine Wave :
  // DAC_Out_Left(DebugWave[debugSoundIdx]);
  // debugSoundIdx = (debugSoundIdx + 1) % 64;
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
  if (!RightChannelFifo_Put(rightSample)) {
    Serial.println("oops1");
  }
  if (!LeftChannelFifo_Put(leftSample)) {
    Serial.println("oops2");
  }
  return true;
}

static unsigned long lastPrint = 0;
const size_t LOW_WATERMARK = 2048;

void loop() {

  // read incoming bytes whenever available
  while (Serial2.available() >= 4) {
    if (!receive()) {
      break;
    }
  }
  // request more when buffer low
  if (LeftChannelCount() < LOW_WATERMARK) {
   // Serial.println(LeftChannelCount());

    Serial2.write('B');
  }

  // occasional debug print
  if (!output && millis() - lastPrint > 250) {
    Serial.println("empty");
    lastPrint = millis();
  }
}
