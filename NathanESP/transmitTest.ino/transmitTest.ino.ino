#define LOW_UART_RX 32

void setup() {
  void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(2000000, SERIAL_8N1, 16, 17);
  //Serial.write("Reading WAV files one byte at a time!");
  Serial2.onReceive(receivedata);

}

void receivedata() {
  if (!Serial2.available()) { 
    return;
  }
  uint8_t lsb = Serial2.read();
  uint8_t msb = Serial2.read();
  int16_t data = (int16_t) ((msb << 8) | lsb));
  Serial.println(data);

}

void loop() {
  Serial2.write('R');
  delay(1000);
}
