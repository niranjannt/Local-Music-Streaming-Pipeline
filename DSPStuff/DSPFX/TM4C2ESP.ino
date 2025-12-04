void ESPUARTsetup() {
  // Code for testing 
  //Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Serial.begin(115200, SERIAL_8N1);
  //
  //Serial0.begin(9600);
}

bool UARTReceive(uint16_t* data) {
    static int state = 0;
    static uint16_t temp;

    while (Serial.available()) {
        //Serial.println("Available Data");
        uint8_t b = Serial.read();

        switch (state) {

        case 0:   // first header byte
            //Serial.println("Case 0");
            if (b == 0xFF) state = 1;
            break;

        case 1:   // second header byte
            //Serial.println("Case 1");
            if (b == 0xFF) {
                state = 2;
            } else {
                state = 0;
            }
            break;

        case 2:   // receiving payload MSB
            //Serial.println("Case 2");
            temp = b << 8;
            state = 3;
            break;

        case 3:   // receiving payload LSB
            //Serial.println("Case 3");
            static int idx = 0;
            temp |= b;
            temp &= 0xFFF;
            data[idx++] = temp;

            if (idx == 10) {
                idx = 0;
                state = 0;
                return true;      // PACKET COMPLETE
            }
            state = 2;
            break;
        }
    }

    return false;
}
