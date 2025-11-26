hw_timer_t* dacTimer = NULL;

void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println(">>>> setup start");

  Serial2.begin(2000000, SERIAL_8N1, 16, 17);
  delay(2000);
  Serial.println("Serial2.begin finished");

  bool waitForSampleRate = false;
  while (waitForSampleRate) {
    uint8_t header;
    if (Serial2.available()) {
      header = Serial2.read();
      waitForSampleRate = (header == 0xC0);
    }
  }

  TimerInit();

  //FIFOINIT
  //DACINIT
}

void TimerInit() {
  while (Serial.availebale() < 2) {}
  uint8_t byte1 = Serial2.read();
  uint8_t byte2 = Serial2.read();
  uint16_t sampleRate = (byte2 << 8) | byte1;

  timerAttachInterrupt(dacTimer, &SoundHandler);
  timerAlarm(dacTimer, (40000000 / sampleRate), true, 0); 
}

void changeSampleRate() {
  uint8_t byte1;
  uint8_t byte2;
  
  RightChannelFifo_Get(&byte1);
  RightChannelFifo_Get(&byte2);
  uint16_t sampleRate = (byte2 << 8) | byte1;
  timerAlarm(dacTimer, (40000000 / sampleRate). true, 0);
}

void receive() {
  uint8_t byte1 = Serial2.read();
  uint8_t byte2 = Serial2.read();
  uint8_t byte3 = Serial2.read();
  uint8_t byte4 = Serial2.read();

  leftSample = (byte2 << 8) | byte1;
  rightSample =(byte4 << 8) | byte3;

  //todo error handling
  RightChannelFiFo_Put(rightSample);
  LeftChannelFifo_Put(leftSample);
}

void loop() {
 if (Serial2.available() >= 4) {
  receive();
 }
}
