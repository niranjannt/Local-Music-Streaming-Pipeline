
#define OMEGA 0.0000654498469497874
int32_t n3 = 0;
#define FLANGER_PERIOD 96000

int16_t reverb_process_left_sample(int16_t input_sample)
{
	int16_t output_sample;
	output_sample = input_sample;

	float_t x;
	float_t y;
  size_t Kn;
  Kn = 0.5*(1 + cos(OMEGA*n));
	x = INPUT_SCALE_FACTOR*input_sample;
	y = x*Kn;
	output_sample = OUTPUT_SCALE_FACTOR*y;
  return output_sample;
}

int16_t reverb_process_right_sample(int16_t input_sample)
{
  int16_t output_sample;
	output_sample = input_sample;
	float_t x;
	float_t y;
  size_t Kn;
  Kn = 0.5*(1 + cos(OMEGA*n));
	x = INPUT_SCALE_FACTOR*input_sample;
  y = x*Kn;
	output_sample = OUTPUT_SCALE_FACTOR*y;
  n3 = (n3+1) % FLANGER_PERIOD;
	return output_sample;
}