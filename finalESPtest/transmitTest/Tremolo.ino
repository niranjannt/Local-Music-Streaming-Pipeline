
uint32_t n_tremolo = 0;
uint16_t TremoloWave[64] = {
  1024, 1122, 1219, 1314, 1407, 1495, 1580, 1658, 1731, 1797, 1855,
  1906, 1948, 1981, 2005, 2019, 2024, 2019, 2005, 1981, 1948, 1906,
  1855, 1797, 1731, 1658, 1580, 1495, 1407, 1314, 1219, 1122, 1024,
  926, 829, 734, 641, 553, 468, 390, 317, 251, 193, 142,
  100, 67, 43, 29, 24, 29, 43, 67, 100, 142, 193,
  251, 317, 390, 468, 553, 641, 734, 829, 926
};
#define TREMOLO_WAVE_PERIOD 64

uint16_t tremolo_process_left_sample(uint16_t input_sample) {

  uint16_t output_sample;
  uint32_t x;
  uint32_t Kn;
  Kn = 0.5(1 + TremoloWave[n]);
  x = input_sample * Kn;

  output_sample = x / (SCALE_FACTOR * 1024);
  n_tremolo = (n_tremolo + 1) % TREMOLO_WAVE_PERIOD;
  return output_sample;
}

uint16_t tremolo_process_right_sample(uint16_t input_sample) {
  
  uint16_t output_sample;
  uint32_t x;
  uint32_t Kn;
  Kn = 0.5(1 + TremoloWave[n]);
  x = input_sample * Kn;

  output_sample = x / (SCALE_FACTOR * 1024);
  return output_sample;
}