/*
 * led.c
 *
 *  Created on: 8 oct. 2018
 *      Author: Nirgal
 */

#include "led.h"
#include "config.h"
#include "stm32f1xx_hal.h"
#include "systick.h"

#define write_led(x)	HAL_GPIO_WritePin(LED_GREEN, x)
volatile static led_state = LED_OFF;

void LED_init(void)
{
	//Initialisation du port de la led Verte (carte Nucleo)
	BSP_GPIO_PinCfg(LED_GREEN,GPIO_MODE_OUTPUT_PP,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);

	Systick_add_callback_function(&LED_process_ms);
}


void LED_set(LED_state_e state)
{

	if(state < LED_STATE_NB)
		led_state = state;
}


void LED_process_ms(void)
{
	static uint16_t local_t;
	local_t = (local_t+1)%128;

	switch(led_state)
	{
		case LED_OFF:
			write_led(0);
			break;
		case LED_ON:
			write_led(1);
			break;
		case LED_BLINK:
			write_led(local_t/64);
			break;
		default:
			break;
	}
}
