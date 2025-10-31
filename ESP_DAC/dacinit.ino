#include <SPI.h>

#define LAUDIO_FS 14
#define RAUDIO_FS 27
#define RAUDIO_CLK 26
#define RAUDIO_OUT 25
#define LAUDIO_CLK 13
#define LAUDIO_OUT 12

SPIClass spileft(VSPI);
SPIClass spiright(HSPI);
void DAC_Init_Left(){
pinMode(LAUDIO_FS, OUTPUT);
digitalWrite(LAUDIO_FS, HIGH);
spileft.begin(LAUDIO_CLK, -1, LAUDIO_OUT, -1);


}

void DAC_Init_Right(){
pinMode(RAUDIO_FS, OUTPUT);
digitalWrite(RAUDIO_FS, HIGH);
spiright.begin(RAUDIO_CLK, -1, RAUDIO_OUT, -1);

}

void DAC_Out_Left(uint16_t left_channel_data){
spileft.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE2));
digitalWrite(LAUDIO_FS, LOW);
spileft.transfer16(left_channel_data);
digitalWrite(LAUDIO_FS, HIGH);
spileft.endTransaction();

}

void DAC_Out_Right(uint16_t right_channel_data){
spiright.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE2));
digitalWrite(RAUDIO_FS, LOW);
spiright.transfer16(right_channel_data);
digitalWrite(RAUDIO_FS, HIGH);
spiright.endTransaction();

}