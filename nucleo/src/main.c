/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_nucleo.h"
#include "stm32f1_uart.h"
#include "stm32f1_sys.h"
#include "stm32f1_gpio.h"
#include "macro_types.h"
#include "led.h"
#include "burger_dialog.h"
#include "button.h"



int main(void)
{
	HAL_Init();					//Initialisation de la couche logicielle HAL (Hardware Abstraction Layer)
	BSP_GPIO_Enable();			//Activation des périphériques GPIO
	SYS_ClockConfig();			//Configuration des horloges.

	//Initialisation de l'UART1 à la vitesse de 115200 bauds/secondes (92kbits/s) PA2 : Tx  | PA3 : Rx.
		//Attention, les pins PA2 et PA3 ne sont pas reliées jusqu'au connecteur de la Nucleo.
		//Ces broches sont redirigées vers la sonde de débogage, la liaison UART étant ensuite encapsulée sur l'USB vers le PC de développement.
	UART_init(UART2_ID,115200);

	//"Indique que les printf sortent vers le périphérique UART2."
	SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

	LED_init();
	BUTTON_init();

	while(1)
	{
		BURGER_DIALOG_process_main();
		BUTTON_process_main();


	}

}





