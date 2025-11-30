#define alpha 0.75
#define R 480
#define BUFFER_LENGTH 481
#define OMEGA 0.0000654498469497874
#define FLANGER_PERIOD 96000

int16_t x_circ[2][BUFFER_LENGTH] = {0};
int32_t i1 = 0;
int32_t i2 = 0;
int32_t n = 0;

int16_t flanger_process_left_sample(int16_t input_sample)
{

	int16_t output_sample;
	i2 = i1;
	i1 = (i1+1) % BUFFER_LENGTH;
	x_circ[0][i2] = input_sample;

	float_t x_current;
	float_t x_delayed;
	size_t Kn;
	size_t ind;

		x_current = INPUT_SCALE_FACTOR*x_circ[0][i2];

		Kn = 0.5*R*(cos(OMEGA*n) + 1.0);

		if (Kn > i2)
		{
		    ind = (i2 + BUFFER_LENGTH) - Kn;
		} else
		{
		    ind = i2 - Kn;
		}

		x_delayed = INPUT_SCALE_FACTOR*x_circ[0][ind];

		output_sample = OUTPUT_SCALE_FACTOR*(alpha*x_delayed + x_current);

	return output_sample;
}

int16_t flanger_process_right_sample(int16_t input_sample)
{

	int16_t output_sample;
	x_circ[1][i2] = input_sample;
		n = (n+1) % FLANGER_PERIOD;

		float_t x_current;
		float_t x_delayed;
		size_t Kn;
		size_t ind;

		x_current = INPUT_SCALE_FACTOR*x_circ[1][i2];

		Kn = 0.5*R*(cos(OMEGA*n) + 1.0);

		if (Kn > i2)
		{
		    ind = (i2 + BUFFER_LENGTH) - Kn;
		} else
		{
		    ind = i2 - Kn;
		}

		x_delayed = INPUT_SCALE_FACTOR*x_circ[1][ind];

		output_sample = OUTPUT_SCALE_FACTOR*(alpha*x_delayed + x_current);

		n = (n+1) % FLANGER_PERIOD;
	return output_sample;
}