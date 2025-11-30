#define alpha 0.75
#define R 480
#define BUFFER_LENGTH 481
#define OMEGA 0.0000654498469497874
#define FLANGER_PERIOD 96000

int16_t x_circ2[2][BUFFER_LENGTH] = {0};
int32_t i12 = 0;
int32_t i22 = 0;
int32_t n2 = 0;

int16_t delay_process_left_sample(int16_t input_sample)
{

	int16_t output_sample;
	i22 = i12;
	i12 = (i12+1) % BUFFER_LENGTH;
	x_circ2[0][i22] = input_sample;

	float_t x_current;
	float_t x_delayed;
	size_t Kn;
	size_t ind;

		x_current = INPUT_SCALE_FACTOR*x_circ2[0][i22];

		Kn = 40;

		if (Kn > i22)
		{
		    ind = (i22 + BUFFER_LENGTH) - Kn;
		} else
		{
		    ind = i22 - Kn;
		}

		x_delayed = INPUT_SCALE_FACTOR*x_circ2[0][ind];

		output_sample = OUTPUT_SCALE_FACTOR*(alpha*x_delayed + x_current);

	return output_sample;
}

int16_t delay_process_right_sample(int16_t input_sample)
{

	int16_t output_sample;
	x_circ2[1][i22] = input_sample;
		//n2 = (n2+1) % FLANGER_PERIOD;

		float_t x_current;
		float_t x_delayed;
		size_t Kn;
		size_t ind;

		x_current = INPUT_SCALE_FACTOR*x_circ2[1][i22];

		Kn = 40;

		if (Kn > i22)
		{
		    ind = (i22 + BUFFER_LENGTH) - Kn;
		} else
		{
		    ind = i22 - Kn;
		}

		x_delayed = INPUT_SCALE_FACTOR*x_circ2[1][ind];

		output_sample = OUTPUT_SCALE_FACTOR*(alpha*x_delayed + x_current);

		n2 = (n2+1) % FLANGER_PERIOD;
	return output_sample;
}