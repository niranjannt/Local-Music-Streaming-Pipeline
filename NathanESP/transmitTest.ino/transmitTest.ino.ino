#define LOW_UART_RX 32

void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println(">>>> setup start");

  Serial2.begin(2000000, SERIAL_8N1, 16, 17);
  delay(2000);
  Serial.println("Serial2.begin finished");

  Serial.println("Reading sample rate!");


}

void receive() {
  Serial.println(Serial2.read(), HEX);
  Serial2.write('R');
}

void loop() {
 if (Serial2.available()) {
  receive();
 }
}
