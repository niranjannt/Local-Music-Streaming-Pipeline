#define CLIP 2048

uint16_t distortion_process_left_sample(uint16_t input_sample) {
  uint16_t output_sample;
  output_sample = input_sample;

  if (output_sample > CLIP) {
    output_sample = CLIP;
  }

  return output_sample;
}


uint16_t distortion_process_right_sample(uint16_t input_sample) {
  uint16_t output_sample;
  output_sample = input_sample;

  if (output_sample > CLIP) {
    output_sample = CLIP;
  }

  return output_sample;
}