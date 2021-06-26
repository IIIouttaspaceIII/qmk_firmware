RGB_MATRIX_EFFECT(CUSTOMFOUR)
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

#include "print.h"

//Need to add in a gamma function to use on time based on if the wave is going in or out
//Wave going in needs to compress on land side, going out will compress to ocean side

//Need a Y effecting offset
//Should effect both waves independantly

//applys the gamma curve from 0 to 1 as 0 to 255
int16_t c_apply_gamma_curve_255_as_1(int16_t x, int8_t gamma)
{
	/*
	//Maybe a simple linear step?
	//offset centre by sin wave left and right?
	if (x < centrepoint)
	{
		//multiply / scale16by8 to get the value
	}
	else
	{
		//Add offset and multiply to get new position
	}*/
	return x;
}

uint8_t c_lerp8(uint8_t x, uint8_t y, uint8_t time)
{
	return scale8(x, 0xFF - time) + scale8(y, time);
}

HSV get_wave_data_at_time(uint8_t time, HSV wave_data_points[])
{
	uint8_t point = time >> 5;
    uint8_t time2 = ((time) & 0b11111) << 3;
	HSV out = {c_lerp8(wave_data_points[point].h, wave_data_points[point + 1].h, time2),
			c_lerp8(wave_data_points[point].s, wave_data_points[point + 1].s, time2), 
			c_lerp8(wave_data_points[point].v, wave_data_points[point + 1].v, time2)};
	return out;
}

uint8_t calculate_wave_theta(uint8_t offset)
{
	return offset + (uint8_t)((timer_read() >> 6) & 0xFF);
}
/*
int8_t calculate_wave_y_theta(uint8_t offset)//Need some additional offset?
{
	return offset + ((((timer_read() >> 5) % 90) * ));//Will break every few minutes
}*/
uint8_t get_wave_offset(uint8_t theta)
{
  uint32_t theta_shifted = ((uint32_t)theta) << 8;
  uint32_t x = (theta_shifted - (255 << 7));
  return (uint8_t)((int32_t)255 - (int32_t)(((x * x) >> (uint32_t)14) / (uint32_t)255));
}
int16_t get_wave_time(uint8_t i, uint8_t theta)
{
	//wave is going in when theta between 0 and 127
	int16_t time = g_led_config.point[i].x + (get_wave_offset(theta) * - 3) / 2 + 60 + 382;
	return time > 0 ? time : 0;
}
//Conditions are one wave is capping against beach or capping and coming in
//else if both are capping then the furtherest is chosen
bool is_wave_cap_drawn(bool wave1_in_cap_region, int16_t wave1_time, bool wave1_coming_in, bool wave2_in_cap_region, int16_t wave2_time, bool wave2_coming_in)
{
	return (wave1_in_cap_region && (wave1_time < wave2_time)) || (wave1_coming_in && wave1_in_cap_region) || (wave1_in_cap_region && wave2_in_cap_region);
}

HSV get_beach_color(uint8_t i)
{
	uint8_t time = g_led_config.point[i].x;
	time += 100;
	time = time < 100 ? 255 : time;
	HSV hsv = {20, c_lerp8(0xFF, 0xEA, time), 0xFF};
	return hsv;
}

bool CUSTOMFOUR(effect_params_t* params) {
	RGB_MATRIX_USE_LIMITS(led_min, led_max);
	HSV wave_data_points[] = {{130, 0xFF, 0x99},{140, 0xFF, 0xBB},{150, 0xFF, 0xCC},{140, 0xFF, 0xDD},{130, 0xE0, 0xEE},{110, 0xD0, 0xFF},{100, 0x70, 0xFF},{100, 50, 0xFF},{100, 0, 0xFF}};

	for (uint8_t i = led_min; i < led_max; i++) 
	{
		//GREATER theta should have less rendering priority
		
		int8_t theta = calculate_wave_theta(0);
		int8_t theta2 = calculate_wave_theta(127);
		
		int16_t time = get_wave_time(i, theta);
		int16_t time2 = get_wave_time(i, theta2);
		
		HSV hsv = get_beach_color(i);
		bool time_is_valid = time < 0xFF;
		bool time2_is_valid = time2 < 0xFF;
		bool time_is_capping = time >= 0xFF && time < (0xFF + 32);
		bool time2_is_capping = time2 >= 0xFF && time2 < (0xFF + 32);
		
		bool wave1_coming_in = theta > theta2;
		bool wave2_coming_in = theta2 > theta;
		
		if (time_is_valid && time2_is_valid)
		{
			//Should probably get hue from the "deepest" wave
			//Unless I move to a descriptive wave solution
			//Wave would have depth and "turbulence"
			//Calculate hue from depth and lum + sat from turbulence
			hsv = get_wave_data_at_time(wave1_coming_in ? time : time2, wave_data_points);
		}
		else if (time_is_valid)
		{
			hsv = get_wave_data_at_time(time, wave_data_points);
		}
		else if (time2_is_valid)
		{
			hsv = get_wave_data_at_time(time2, wave_data_points);
		}
		//Add smooth wave capping
		//lerp in RGB rather than hsv to stop colour shifting
		RGB rgb;
		//Conditions are one wave is capping against beach or capping and coming in
		//else if both are capping then the furtherest is chosen
		//Make sure this works with scaling value
		bool wave1_is_drawn = is_wave_cap_drawn(time_is_capping, time, wave1_coming_in, time2_is_capping, time2, wave2_coming_in);
		bool wave2_is_drawn = is_wave_cap_drawn(time2_is_capping, time2, wave2_coming_in, time_is_capping, time, wave1_coming_in);
		if (wave1_is_drawn || wave2_is_drawn)
		{
			//Which one is capping
			uint8_t cap_time;
			if (wave1_is_drawn && wave2_is_drawn)
			{
				cap_time = time < time2 ? time : time2;
			}
			else if (wave1_is_drawn)
			{
				cap_time = time;
			}
			else
			{
				cap_time = time2;
			}
			
			cap_time = ((cap_time + 1) & 0b11111) << 3;
			HSV wave_hsv = {wave_data_points[8].h, scale8(wave_data_points[8].s, rgb_matrix_config.hsv.s), scale8(wave_data_points[8].v, rgb_matrix_config.hsv.v)};
			RGB crest = rgb_matrix_hsv_to_rgb(wave_hsv);
			//need to scale sat and val of BOTH the inputs
			hsv.s = scale8(hsv.s, rgb_matrix_config.hsv.s);
			hsv.v = scale8(hsv.v, rgb_matrix_config.hsv.v);
			rgb = rgb_matrix_hsv_to_rgb(hsv);
			rgb.r = c_lerp8(crest.r, rgb.r, cap_time);
			rgb.g = c_lerp8(crest.g, rgb.g, cap_time); 
			rgb.b = c_lerp8(crest.b, rgb.b, cap_time);
		}
		else
		{
			hsv.s = scale8(hsv.s, rgb_matrix_config.hsv.s);
			hsv.v = scale8(hsv.v, rgb_matrix_config.hsv.v);
				
			rgb = rgb_matrix_hsv_to_rgb(hsv);
		}

		rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
	}
	
    return led_max < DRIVER_LED_TOTAL;
}

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS