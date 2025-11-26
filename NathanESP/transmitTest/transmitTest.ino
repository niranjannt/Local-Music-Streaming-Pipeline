hw_timer_t* dacTimer = NULL;
static void IRAM_ATTR soundHandler();

void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println(">>>> setup start");

  Serial2.begin(2000000, SERIAL_8N1, 16, 17);
  delay(2000);
  Serial.println("Serial2.begin finished");

  bool waitForSampleRate = true;
  while (waitForSampleRate) {
    uint8_t header;
    if (Serial2.available()) {
      header = Serial2.read();
      waitForSampleRate = !(header == 0xC0);
    }
  }

  TimerInit();

  RightChannelFifo_Init();
  LeftChannelFifo_Init();
  DAC_Init_Left();
  DAC_Init_Right();
}

void TimerInit() {
  Serial.println("Timer Init");
  dacTimer = timerBegin(40000000);
  while (Serial2.available() < 2) {}
  Serial.println("Reading sample rate: ");
  uint8_t byte1 = Serial2.read();
  uint8_t byte2 = Serial2.read();
  uint16_t sampleRate = (byte2 << 8) | byte1;
  Serial.println(sampleRate, HEX);

  timerAttachInterrupt(dacTimer, &soundHandler);
  timerAlarm(dacTimer, (40000000 / sampleRate), true, 0); 
}

void changeSampleRate() {
  uint16_t sampleRate; 
  RightChannelFifo_Get(&sampleRate);
  timerAlarm(dacTimer, (40000000 / sampleRate), true, 0);
}

const uint16_t Wave[64] = {  
  1024,1122,1219,1314,1407,1495,1580,1658,1731,1797,1855,
  1906,1948,1981,2005,2019,2024,2019,2005,1981,1948,1906,
  1855,1797,1731,1658,1580,1495,1407,1314,1219,1122,1024,
  926,829,734,641,553,468,390,317,251,193,142,
  100,67,43,29,24,29,43,67,100,142,193,
  251,317,390,468,553,641,734,829,926
};  


uint16_t soundIdx = 0;
static void IRAM_ATTR soundHandler() {
  // uint16_t leftData;
  // uint16_t rightData;

  // LeftChannelFifo_Get(&leftData);
  // RightChannelFifo_Get(&rightData);
  // DAC_Out_Left(leftData);
  // Serial2.write("r");
  DAC_Out_Left(Wave[soundIdx]);
  soundIdx = (soundIdx + 1) % 64;
}

void receive() {
  uint8_t byte1 = Serial2.read();
  uint8_t byte2 = Serial2.read();
  uint8_t byte3 = Serial2.read();
  uint8_t byte4 = Serial2.read();

  uint16_t leftSample = (byte2 << 8) | byte1;
  uint16_t rightSample = (byte4 << 8) | byte3;

  //todo error handling
  RightChannelFifo_Put(rightSample);
  LeftChannelFifo_Put(leftSample);
}

void loop() {
 if (Serial2.available() >= 4) {
  receive();
 }
}
