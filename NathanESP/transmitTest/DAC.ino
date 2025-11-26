#include <SPI.h>

#define LAUDIO_FS 14
#define RAUDIO_FS 27
#define RAUDIO_CLK 26
#define RAUDIO_OUT 25
#define LAUDIO_CLK 13
#define LAUDIO_OUT 12

SPIClass spiLeft(VSPI);
SPIClass spiRight(HSPI);
void DAC_Init_Left(){  
  pinMode(LAUDIO_FS, OUTPUT);
  digitalWrite(LAUDIO_FS, HIGH);
  spiLeft.begin(LAUDIO_CLK, -1, LAUDIO_OUT, -1);
}

void DAC_Init_Right(){
  pinMode(RAUDIO_FS, OUTPUT);
  digitalWrite(RAUDIO_FS, HIGH);
  spiRight.begin(RAUDIO_CLK, -1, RAUDIO_OUT, -1);
}

void DAC_Out_Left(uint16_t left_channel_data){
  spiLeft.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE2));
  digitalWrite(LAUDIO_FS, LOW);
  spiLeft.transfer16(0x4000|(left_channel_data&0x0fff));
  digitalWrite(LAUDIO_FS, HIGH);
  spiLeft.endTransaction();
}

void DAC_Out_Right(uint16_t right_channel_data){
  spiRight.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE2));
  digitalWrite(RAUDIO_FS, LOW);
  spiRight.transfer16(0x4000|(right_channel_data&0x0fff));
  digitalWrite(RAUDIO_FS, HIGH);
  spiRight.endTransaction();
}