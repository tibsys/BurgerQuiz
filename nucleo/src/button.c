/*
 * button.c
 *
 *  Created on: 8 oct. 2018
 *      Author: Nirgal
 */
#include "button.h"
#include "config.h"
#include "stm32f1xx_hal.h"
#include "burger_dialog.h"

volatile static bool_e flag_button_press_event = FALSE;

void BUTTON_init(void)
{
	//Initialisation du port du bouton bleu (carte Nucleo)
	BSP_GPIO_PinCfg(BLUE_BUTTON,GPIO_MODE_INPUT,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);

	Systick_add_callback_function(&BUTTON_process_ms);
}


void BUTTON_process_ms(void)
{
	static bool_e previous_button = FALSE;
	bool_e current_button;

	current_button = !HAL_GPIO_ReadPin(BLUE_BUTTON);
	if(current_button && !previous_button)
	{
		flag_button_press_event = TRUE;
	}
	previous_button = current_button;
}

void BUTTON_process_main(void)
{
	if(flag_button_press_event)
	{
		flag_button_press_event = FALSE;
		BURGER_DIALOG_send_msg(MSG_BUTTON, 0, NULL);
	}
}
