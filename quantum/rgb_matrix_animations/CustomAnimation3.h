#ifndef DISABLE_RGB_MATRIX_CUSTOMTHREE
RGB_MATRIX_EFFECT(CUSTOMTHREE)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

#        ifndef RGB_MATRIX_COLOR2_UPDATE_DELAY_MS
#            define RGB_MATRIX_COLOR2_UPDATE_DELAY_MS 12000
#        endif

static uint32_t CUSTOMTHREE_timer;
static uint8_t CUSTOMTHREEhue;
static uint8_t CUSTOMTHREEhueGoal;


uint8_t CustomThree_get_hue(uint8_t index, uint8_t timer_fract)
{
	return timer_fract < g_led_config.point[index].x ? CUSTOMTHREEhue : CUSTOMTHREEhueGoal;
}

uint8_t CustomThree_get_sat(uint8_t index, uint8_t timer_fract)
{
	uint8_t val = g_led_config.point[index].x - timer_fract;
	val = scale8(val,val);
	val = scale8(val,val);
	val = -scale8(val,val);
	return timer_fract - g_led_config.point[index].x;
}

uint8_t CustomThree_get_timer_fract(uint32_t elapsed)
{
	return ((uint64_t)elapsed * 255) / RGB_MATRIX_COLOR2_UPDATE_DELAY_MS;
}

bool CUSTOMTHREE(effect_params_t* params) {
	
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
	uint32_t elapsed = timer_elapsed32(CUSTOMTHREE_timer);
	
	if (elapsed >= RGB_MATRIX_COLOR2_UPDATE_DELAY_MS)
	{
		CUSTOMTHREEhue = CUSTOMTHREEhueGoal;
		CUSTOMTHREEhueGoal = rand() & 0xFF;
		CUSTOMTHREE_timer = timer_read32();
	}
	uint8_t timer_fract = CustomThree_get_timer_fract(elapsed);

	for (uint8_t i = led_min; i < led_max; i++) {
		uint8_t sat = CustomThree_get_sat(i, timer_fract);
		HSV hsv = {CustomThree_get_hue(i, timer_fract), scale8(sat, rgb_matrix_config.hsv.s) , rgb_matrix_config.hsv.v};
		RGB rgb = rgb_matrix_hsv_to_rgb(hsv);		//RGB_MATRIX_TEST_LED_FLAGS();
		
		rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
		//rgb_matrix_set_color(i, 0, 255, 255);
	}
	
    return led_max < DRIVER_LED_TOTAL;
}

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // DISABLE_RGB_MATRIX_BREATHING