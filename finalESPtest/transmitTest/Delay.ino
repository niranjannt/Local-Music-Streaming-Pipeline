#define delay_alpha 0.5
#define DELAY_BUFFER_LENGTH 1000

uint16_t delay_circ[2][DELAY_BUFFER_LENGTH] = {0};

uint16_t i1_delay = 0;
uint16_t i2_delay = 0;

uint16_t delay_process_left_sample(uint16_t input_sample, uint16_t delay_time) {
  uint16_t output_sample;
  i2_delay = i1_delay;
  i1_delay = (i2_delay + 1) % BUFFER_LENGTH;
  delay_circ[0][i2_delay] = input_sample;

  uint32_t x_current;
  uint32_t x_delayed;
  uint32_t Kn;
  uint32_t ind;

  x_current = delay_circ[0][i2_delay] / SCALE_FACTOR;

  Kn = delay_time;

  if (Kn > i2_delay) {
    ind = (i2_delay + BUFFER_LENGTH) - Kn;
  } 
  else {
    ind = i2_delay - Kn;
  }

  x_delayed = delay_circ[0][ind] / SCALE_FACTOR;

  output_sample = (delay_alpha*x_delayed) + x_current;

  return output_sample;
}

uint16_t delay_process_right_sample(uint16_t input_sample, uint16_t delay_time) {
  uint16_t output_sample;
  
  delay_circ[0][i2_delay] = input_sample;

  uint32_t x_current;
  uint32_t x_delayed;
  uint32_t Kn;
  uint32_t ind;

  x_current = delay_circ[1][i2_delay] / SCALE_FACTOR;

  Kn = delay_time;

  if (Kn > i2_delay) {
    ind = (i2_delay + BUFFER_LENGTH) - Kn;
  } 
  else {
    ind = i2_delay - Kn;
  }

  x_delayed = delay_circ[1][ind] / 2;

  output_sample = (alpha*x_delayed) + x_current;

  return output_sample;
}