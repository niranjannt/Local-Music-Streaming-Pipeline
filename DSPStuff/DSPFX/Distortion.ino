#define CLIP 0.1


int16_t distortion_process_left_sample(int16_t input_sample)
{
	int16_t output_sample;
	output_sample = input_sample;

	float_t x;
	float_t y;
	x = INPUT_SCALE_FACTOR*input_sample;
	if (x < -CLIP)
	{
		y = -CLIP;
	}
	else if ( x > CLIP )
	{
		y = CLIP;
	}
	else
	{
		y = x;
	}
	output_sample = OUTPUT_SCALE_FACTOR*y;
  return output_sample;
}

int16_t distortion_process_right_sample(int16_t input_sample)
{
  int16_t output_sample;
	output_sample = input_sample;
	float_t x;
	float_t y;
	x = INPUT_SCALE_FACTOR*input_sample;
	if (x < -CLIP)
	{
		y = -CLIP;
	}
	else if ( x > CLIP )
	{
		y = CLIP;
	}
	else
	{
		y = x;
	}
	output_sample = OUTPUT_SCALE_FACTOR*y;
	return output_sample;
}