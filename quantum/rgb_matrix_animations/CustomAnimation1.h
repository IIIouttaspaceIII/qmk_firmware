#ifndef DISABLE_RGB_MATRIX_CUSTOMONE
RGB_MATRIX_EFFECT(CUSTOMONE)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

bool CUSTOMONE(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    HSV      hsv  = rgb_matrix_config.hsv;
    //uint16_t time = scale16by8(g_rgb_timer, rgb_matrix_config.speed / 8);
    //hsv.h         = scale8(abs8(time - 128) * 2, hsv.v);
    //RGB rgb       = rgb_matrix_hsv_to_rgb(hsv);
	
	
    for (uint8_t i = led_min; i < led_max; i++) {
		hsv.h         = rand() % 256;
		RGB rgb       = rgb_matrix_hsv_to_rgb(hsv);
        RGB_MATRIX_TEST_LED_FLAGS();
		
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
		//rgb_matrix_set_color(i, rand() % 256, rand() % 256, rand() % 256);
    }
    return led_max < DRIVER_LED_TOTAL;
}

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // DISABLE_RGB_MATRIX_BREATHING
