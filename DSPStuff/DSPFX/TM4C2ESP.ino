bool foundHEADER;
uint32_t HEADER;

void ESPUARTsetup() {
  HEADER = 0xFFFF;
  foundHEADER = false;
  Serial1.begin(0, SERIAL_8N1, -1, -1, false, 20000);
}

void UARTRecieve(uint32_t* data) {
  uint32_t header2compare = 0;
  if (foundHEADER) {
    while (Serial1.available() < 20) {}
    for (int i = 0; i < 10; i++) {
      data[i] = Serial1.read();
      data[i] = data[i] + (Serial1.read() << 8);
    }
    foundHEADER = false;
  }
  else { // finding header
    while (Serial1.available() < 2) {}
    header2compare = Serial1.read();
    header2compare = header2compare + (Serial1.read() << 8);
    if (header2compare == HEADER) {
      foundHEADER = true;
    }
  }
}
