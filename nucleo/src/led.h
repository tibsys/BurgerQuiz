/*
 * led.h
 *
 *  Created on: 8 oct. 2018
 *      Author: Nirgal
 */

#ifndef LED_H_
#define LED_H_

typedef enum
{
	LED_OFF = 0,
	LED_ON = 1,
	LED_BLINK = 2,
	LED_STATE_NB
}LED_state_e;

void LED_init(void);
void LED_set(LED_state_e state);
void LED_process_ms(void);

#endif /* LED_H_ */
