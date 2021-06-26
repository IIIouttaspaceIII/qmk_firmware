#ifndef DISABLE_RGB_MATRIX_CUSTOMTWO
RGB_MATRIX_EFFECT(CUSTOMTWO)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

#        ifndef RGB_MATRIX_COLOR_UPDATE_DELAY_MS
#            define RGB_MATRIX_COLOR_UPDATE_DELAY_MS 1000
#        endif

static uint16_t CUSTOMTWO_decrease_timer;
static uint8_t CUSTOMTWOhue;

bool CUSTOMTWO(effect_params_t* params) {
	
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
	uint16_t elapsed = timer_elapsed(CUSTOMTWO_decrease_timer);
	
	
	if (elapsed >= RGB_MATRIX_COLOR_UPDATE_DELAY_MS)
	{
		CUSTOMTWOhue = rand() & 0xFF;
		CUSTOMTWO_decrease_timer = timer_read();
	}
	uint8_t sat = ((uint32_t)elapsed * 255) / RGB_MATRIX_COLOR_UPDATE_DELAY_MS;
	HSV hsv = {CUSTOMTWOhue, scale8(sat, rgb_matrix_config.hsv.s) , rgb_matrix_config.hsv.v};
	RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
	for (uint8_t i = led_min; i < led_max; i++) {
		//RGB_MATRIX_TEST_LED_FLAGS();
		
		rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
		//rgb_matrix_set_color(i, 0, 255, 255);
	}
	
    return led_max < DRIVER_LED_TOTAL;
}

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // DISABLE_RGB_MATRIX_BREATHING
