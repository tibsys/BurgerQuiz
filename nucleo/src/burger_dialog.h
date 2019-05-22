/*
 * burger_dialog.h
 *
 *  Created on: 8 oct. 2018
 *      Author: Nirgal
 */

#ifndef BURGER_DIALOG_H_
#define BURGER_DIALOG_H_

typedef enum
{
	MSG_PING = 0x16,
	MSG_PONG = 0x06,
	MSG_LED = 0xED,
	MSG_BUTTON = 0xB0
}msg_id_e;

#define SOH		0x01		//Start Of Header
#define EOT		0x04		//End Of Transmission


void BURGER_DIALOG_process_main(void);
void BURGER_DIALOG_send_msg(msg_id_e id, uint8_t size, uint8_t * datas);

#endif /* BURGER_DIALOG_H_ */
