/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2015
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "WizFi360_ll.h"

uint8_t WizFi360_LL_USARTInit(uint32_t baudrate) {
	/* Init USART */
	TM_USART_Init(WizFi360_USART, WizFi360_USART_PP, baudrate);
	
	/* Return 0 = Successful */
	return 0;
}

uint8_t WizFi360_LL_USARTSend(uint8_t* data, uint16_t count) {
	/* Send data via USART */
	TM_USART_Send(WizFi360_USART, data, count);
	
	/* Return 0 = Successful */
	return 0;
}

/* USART receive interrupt handler */
void TM_USART1_ReceiveHandler(uint8_t ch) {
	/* Send received character to ESP stack */
	WizFi360_DataReceived(&ch, 1);
}
