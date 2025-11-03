    //#include "esp32-hal-timer.h"
#define FIFOSIZE 4096

hw_timer_t* dacTimer = NULL;
//uint8_t index=0;
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
Serial2.begin(2000000, SERIAL_8N1, 16, 17);
//Serial.write("Reading WAV files one byte at a time!");
Serial2.onReceive(receivedata);
pinMode(LED_BUILTIN, OUTPUT);

DAC_Init_Left();
DAC_Init_Right();
RightChannelFifo_Init();
LeftChannelFifo_Init();
}
volatile boolean stopmusic=false;
volatile uint32_t ledCounter = 0;

volatile boolean requestdata=false;
void IRAM_ATTR PeriodicDacOut(){
  if(LeftChannelisEmpty()==0 && !stopmusic){
   uint16_t data_received_left=LeftChannelFifo_Get();
   DAC_Out_Left(data_received_left);
    requestdata=true;
    //Serial.println(requestdata ? "true" : "false"); 


  }
  if(RightChannelisEmpty()==0  && !stopmusic){
   uint16_t data_received_right=RightChannelFifo_Get();
   DAC_Out_Right(data_received_right);
    requestdata=true;
    //Serial.println(requestdata ? "true" : "false"); 


  } 

    ledCounter++;
  if (ledCounter >= 10000) {   // adjust this number for visible blinking
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    ledCounter = 0;
  }


}



void setSampleRate(uint16_t samplerate){
dacTimer= timerBegin(40000000);
if (dacTimer == NULL) {
    Serial.println("Timer setup failed!");
} else {
    Serial.print("Timer configured successfully, pointer = 0x");
    Serial.println((uintptr_t)dacTimer, HEX);  // prints pointer in hex
}
timerAttachInterrupt(dacTimer, &PeriodicDacOut);
timerAlarm(dacTimer, (40000000/samplerate), true, 0);






}

void receivedata(){
   if(Serial2.available()<2){ return;} 
   uint8_t header= Serial2.read();
  Serial.println(header, HEX);
  if(header==0xC0){
   if(Serial2.available()<2){ return; }
  uint8_t lsbsample=Serial2.read();
  Serial.println(lsbsample, HEX);
 // uint16_t sixteenbit=((uint16_t) (msb<<8) | lsb);
    uint8_t msbsample=Serial2.read();
    Serial.println(msbsample, HEX);
    uint16_t samplerate=((uint16_t) (msbsample<<8) | lsbsample);
    Serial.println(samplerate, HEX);
    setSampleRate(samplerate);
   }
   
  
  
  else{
     uint8_t msbsampledata= Serial2.read();                  
     uint16_t sampledata= ((uint16_t) (msbsampledata<<8) | header);
     //Serial.println(sampledata, HEX);
     uint16_t dataheader = ((sampledata&49152) >> 14);
     //Serial.println(dataheader, HEX);
     uint16_t twelvebitdata= ((sampledata & 0x3FFF) >> 2);
     Serial.println(twelvebitdata, HEX);
       switch(dataheader){
       case 0: stopmusic=true; break;
       case 1: if(!stopmusic){
       //Serial.println(stopmusic ? "true" : "false");
        RightChannelFifo_Put(twelvebitdata); 
       } 
       break;
       case 2: if(!stopmusic){
       //Serial.println(stopmusic ? "true" : "false"); 
       LeftChannelFifo_Put(twelvebitdata); 
       }
       break;
  }
  }
  }
 
  //uint16_t header= ((sixteenbit & 49152) >> 14);
 // Serial.println(header, HEX);
 // uint16_t data= ((sixteenbit & 0x3FFF) >> 2);
 // Serial.println(data, HEX);

  void printFifoContents(uint16_t* fifo, uint32_t getIndex, uint32_t putIndex, const char* name, uint16_t count = 32) {
    Serial.print(name);
    Serial.println(" contents:");
    for (uint16_t i = 0; i < count; i++) {
        uint32_t index = (getIndex + i) & (FIFOSIZE-1);
        Serial.print("["); Serial.print(index); Serial.print("]: ");
        Serial.println(fifo[index], HEX);
    }
}




void loop() {
  // put your main code here, to run repeatedly:
 if(requestdata){
   Serial2.write('R');
   Serial.write("Sending request bit");
   //Serial2.flush(); // ensures it is sent
   requestdata=false;


 }
//printRightChannelFifo();
//printLeftChannelFifo();
  // static unsigned long lastPrint = 0;
  // if (millis() - lastPrint > 1000) {
  //     printFifoContents(RightChannelFifo, FIFOgetRight, FIFOputRight, "RightChannelFifo");
  //    printFifoContents(LeftChannelFifo, FIFOgetLeft, FIFOputLeft, "LeftChannelFifo");
  //    lastPrint = millis();
   // }

}




