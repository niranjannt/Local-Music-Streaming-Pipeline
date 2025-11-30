bool foundHEADER;
uint32_t HEADER;

void ESPUARTsetup() {
  HEADER = 0xFFFF;
  foundHEADER = false;
  Serial.begin(9600);
}

void UARTRecieve(uint32_t* data) {
  uint32_t header2compare = 0;
  if (foundHEADER) {
    while (Serial.available() < 20) {}
    for (int i = 0; i < 10; i++) {
      data[i] = Serial.read();
      data[i] = data[i] + (Serial.read() << 8);
    }
    foundHEADER = false;
  }
  else { // finding header
    while (Serial.available() < 2) {}
    header2compare = Serial.read();
    header2compare = header2compare + (Serial.read() << 8);
    if (header2compare == HEADER) {
      foundHEADER = true;
    }
  }
}