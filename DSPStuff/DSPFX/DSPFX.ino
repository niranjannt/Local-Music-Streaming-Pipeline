uint16_t inputdata[10];
bool flangerFlag;
bool pauseFlag;
bool distortionFlag;
bool reverbFlag;
bool delayFlag;
bool tempoFlag;

#define INPUT_SCALE_FACTOR 0.0003125
#define OUTPUT_SCALE_FACTOR 3200.0

void setup() {
  // put your setup code here, to run once:
  ESPUARTsetup();
  for (int i = 0; i < 10; i++) {
    inputdata[i] = 0;
  }
  flangerFlag = false;
  pauseFlag = false;
  distortionFlag = false;
  reverbFlag = false;
  delayFlag = false;
  tempoFlag = false;
}
void loop() { // assuming left and right samples sent

  if (UARTReceive(&inputdata[0])) {
    if (inputdata[9] & 0x1) { // Pause Button
      pauseFlag = !pauseFlag;
    }
    if (inputdata[9] & 0x4) { // Flanger Button
      flangerFlag = !flangerFlag;
    }
    if (inputdata[9] & 0x8) { // Distortion Button
      distortionFlag = !distortionFlag;
    }
    if (inputdata[9] & 0x10) { // Reverb Button
      reverbFlag = !reverbFlag;
    }
    if (inputdata[9] & 0x20) { // Tempo Button
      tempoFlag = !tempoFlag;
    }
    if (inputdata[9] & 0x80) { // Delay Button
      delayFlag = !delayFlag;
    }
  }


  if (flangerFlag) {
    //outputsampledata = flanger_process_left_sample(outputsampledata);
  }
  if (tempoFlag) {

  }
  if (pauseFlag) {

  }
  if (delayFlag) {
    //outputsampledata = delay_process_left_sample(outputsampledata);
  }
  if (reverbFlag) {
    
  }
  if (distortionFlag) {
    //outputsampledata = distortion_process_left_sample(outputsampledata); // if we have stereo, have a left and right
  }
  //return outputsampledata;
}
