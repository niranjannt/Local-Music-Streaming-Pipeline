/*void setup(){
Serial.begin(115200);
DAC_Init_Left();
DAC_Init_Right();



}
// 12-bit sine lookup table (0–4095 range), 64 samples
const uint16_t WaveSmall[64] = {  
  1024,1122,1219,1314,1407,1495,1580,1658,1731,1797,1855,
  1906,1948,1981,2005,2019,2024,2019,2005,1981,1948,1906,
  1855,1797,1731,1658,1580,1495,1407,1314,1219,1122,1024,
  926,829,734,641,553,468,390,317,251,193,142,
  100,67,43,29,24,29,43,67,100,142,193,
  251,317,390,468,553,641,734,829,926
};  


// Add this definition above your setup()

void loop(){
  static uint32_t index = 0;
  
  DAC_Out_Left(WaveSmall[index]);
  DAC_Out_Right(WaveSmall[index]);
  
  // This is now safe. SINE_TABLE_SIZE will always be correct (96).
  index = (index + 1) % 64; 
  
  //delayMicroseconds(22); // ~44.1 kHz sample rate
}

*/