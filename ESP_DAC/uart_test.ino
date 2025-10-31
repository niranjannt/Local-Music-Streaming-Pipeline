    //#include "esp32-hal-timer.h"
/*hw_timer_t* dacTimer = NULL;
//uint8_t index=0;
#define LOW_UART_RX 32
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
Serial2.begin(2000000, SERIAL_8N1, 16, 17);
//Serial.write("Reading WAV files one byte at a time!");
Serial2.onReceive(receivedata);

DAC_Init_Left();
DAC_Init_Right();
RightChannelFifo_Init();
LeftChannelFifo_Init();
}

volatile boolean requestdata=false;
void IRAM_ATTR PeriodicDacOut(){
  if(RightChannelisEmpty()==1){
   uint16_t data_received_left=LeftChannelFifo_Get();
   DAC_Out_Left(data_received_left);
    requestdata=true;


  }
  if(LeftChannelisEmpty()==1){
   uint16_t data_received_right=RightChannelFifo_Get();
   DAC_Out_Right(data_received_right);
    requestdata=true;
   

  } 
   
}



void setSampleRate(uint16_t samplerate){
dacTimer= timerBegin(80000000);
timerAttachInterrupt(dacTimer, &PeriodicDacOut);
timerAlarm(dacTimer, (80000000/samplerate), true, 0);







}


void receivedata(){
   if(Serial2.available()>=2){ 
   uint8_t lsb= Serial2.read();
  uint8_t msb=Serial2.read();
  int16_t datasigned=((int16_t) (msb<<8) | lsb);
  uint16_t header= (datasigned&49152)>>14;
  uint16_t data= (datasigned & 0x3FFF);
  switch(header){
  case 0: while(1); break;
  case 1: RightChannelFifo_Put(data); break;
  case 2: LeftChannelFifo_Put(data); break;
  case 3: 
  uint8_t lsb2= Serial2.read();
  uint8_t msb2=Serial2.read();
  uint16_t samplerate= ((uint16_t)(msb2<<8) | lsb2);
  Serial.println("Received Sample Rate");
  Serial.println(samplerate);
  setSampleRate(samplerate);
  break;
  }
}
}

void loop() {
  // put your main code here, to run repeatedly:
 if(requestdata){
   Serial2.write('R');
   requestdata=false;


 }


}

