/*
 * Nora.c
 *
 *  Created on: Dec 1, 2025
 *      Author: nikhil
 */


#include "Nora.h"
#include "main.h"

void Nora_startup(){
	  char str_buffer3[500];
	  sprintf(str_buffer3,"\r\n _______________________ \r\n NEW RUN \r\n _______________________ \r\n\0");
	  HAL_UART_Transmit(getDebugPort(),(uint8_t*)str_buffer3,strlen(str_buffer3),5000);
	  uint8_t rx[30];
	  HAL_GPIO_WritePin(Switch1_GPIO_Port, Switch1_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(Switch1_GPIO_Port, Switch1_Pin, GPIO_PIN_SET);

	  HAL_GPIO_WritePin(GPIO_J9_GPIO_Port, GPIO_J9_Pin, GPIO_PIN_RESET);

	  // Boot into AT mode
//	  HAL_GPIO_WritePin(GPIO_J9_GPIO_Port, GPIO_J9_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(NReset_GPIO_Port, NReset_Pin, GPIO_PIN_RESET);
	  HAL_Delay(10);
	  HAL_GPIO_WritePin(NReset_GPIO_Port, NReset_Pin, GPIO_PIN_SET);
	//  HAL_Delay(10);
	//  HAL_GPIO_WritePin(NReset_GPIO_Port, NReset_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	  HAL_Delay(2000);
	//  HAL_GPIO_WritePin(GPIO_J9_GPIO_Port, GPIO_J9_Pin, GPIO_PIN_SET);

	  HAL_UART_Receive(getNoraPort(), rx, sizeof(rx), 1000);
	  HAL_UART_Transmit(getDebugPort(), rx, strlen((char*)rx), 500);

}

void Nora_command(const char *cmd) {
	  uint8_t rx[30];
	  HAL_UART_Transmit(getNoraPort(), (uint8_t*)cmd, strlen(cmd), 50);
	  HAL_Delay(300);
	  // Receive response
	  int rec = HAL_UART_Receive(getNoraPort(), rx, sizeof(rx), 50);
	  char str_buffer2[500];
	  sprintf(str_buffer2,"\nRead Out from %s %d characters: \r\n\0",cmd,rec);
	  HAL_UART_Transmit(getDebugPort(), str_buffer2, strlen(str_buffer2), 500);
	  HAL_UART_Transmit(getDebugPort(), rx, rec, 500);

}
