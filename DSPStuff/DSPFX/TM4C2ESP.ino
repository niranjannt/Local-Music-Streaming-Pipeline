bool foundHEADER;
uint32_t HEADER;

void ESPUARTsetup() {
  HEADER = 0xFFFF;
  foundHEADER = false;
  // Code for testing 
  Serial2.begin(2400, SERIAL_8N1, 16, 17);
  Serial.begin(9600);
  //
  //Serial0.begin(9600);
}

// Test function, should recieve TM4C data[9] first, then descend. So TM4C data[9] is ESP data[0].
void UARTRecieve(uint32_t* data) {
  uint32_t header2compare = 0;
  if (foundHEADER) {
    while (Serial2.available() < 20) {}
    for (int i = 0; i < 10; i++) {
      data[i] = Serial2.read();
      data[i] = data[i] + (Serial2.read() << 8);
    }
    Serial.println("Received Data");
    foundHEADER = false;
  }
  else { // finding header
    while (Serial2.available() < 2) {}
    header2compare = Serial2.read();
    header2compare = header2compare + (Serial2.read() << 8);
    if (header2compare == HEADER) {
      foundHEADER = true;
    }
  }
}


/*
void UARTRecieve(uint32_t* data) {
  uint32_t header2compare = 0;
  if (foundHEADER) {
    while (Serial0.available() < 20) {}
    for (int i = 0; i < 10; i++) {
      data[i] = Serial0.read();
      data[i] = data[i] + (Serial0.read() << 8);
    }
    foundHEADER = false;
  }
  else { // finding header
    while (Serial0.available() < 2) {}
    header2compare = Serial0.read();
    header2compare = header2compare + (Serial0.read() << 8);
    if (header2compare == HEADER) {
      foundHEADER = true;
    }
  }
}*/
