void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
Serial2.begin(9600, SERIAL_8N1, 16, 17);
Serial.write("Reading WAV files one byte at a time!");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial2.available()){
   char onebyte= Serial2.read();
   Serial.print(onebyte); 
  }

}
