/*
 * Nora.c
 *
 *  Created on: Dec 1, 2025
 *      Author: nikhil
 */


#include "Nora.h"
#include "main.h"

// Print each byte in hex to debug UART
static void debug_print_hex(uint8_t *buf, int len) {
    char out[8];

    for (int i = 0; i < len; i++) {
        // Format as two-digit hex: "AB "
        sprintf(out, "%02X ", buf[i]);
        HAL_UART_Transmit(getDebugPort(), (uint8_t *)out, strlen(out), 100);
    }

    // Newline afterwards
    sprintf(out, "\r\n");
    HAL_UART_Transmit(getDebugPort(), (uint8_t *)out, strlen(out), 100);
}
void Nora_startup(){
    uint8_t rx[256];
    char str_buffer[500];

    sprintf(str_buffer,"\r\n _______________________ \r\n NEW RUN \r\n _______________________ \r\n");
    HAL_UART_Transmit(getDebugPort(),(uint8_t*)str_buffer,strlen(str_buffer),5000);

    // FORCE both switches LOW (bootloader mode)
    HAL_GPIO_WritePin(Switch1_GPIO_Port, Switch1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Switch2_GPIO_Port, Switch2_Pin, GPIO_PIN_RESET);
    HAL_Delay(50);

    // Reset the module with switches LOW
    HAL_GPIO_WritePin(NReset_GPIO_Port, NReset_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(NReset_GPIO_Port, NReset_Pin, GPIO_PIN_SET);

    sprintf(str_buffer, "Module in bootloader, waiting 15s with COMPLETE UART silence...\r\n");
    HAL_UART_Transmit(getDebugPort(),(uint8_t*)str_buffer,strlen(str_buffer),5000);

    // DO NOT touch NORA UART at all - complete silence for 15 seconds
    HAL_Delay(15000);

    sprintf(str_buffer, "Timeout complete, module should have auto-booted to u-connectXpress\r\n");
    HAL_UART_Transmit(getDebugPort(),(uint8_t*)str_buffer,strlen(str_buffer),5000);

    // NOW we can check for startup message
    // Wait a bit for the +STARTUP after reboot
    HAL_Delay(2000);

    memset(rx, 0, sizeof(rx));
    int rec = HAL_UART_Receive(getNoraPort(), rx, sizeof(rx), 500);

    if(rec > 0) {
        sprintf(str_buffer, "Received %d bytes:\r\n", rec);
        HAL_UART_Transmit(getDebugPort(), (uint8_t*)str_buffer, strlen(str_buffer), 500);
        debug_print_hex(rx, rec);
        HAL_UART_Transmit(getDebugPort(), rx, rec, 500);

        if(strstr((char*)rx, "+STARTUP") != NULL) {
            sprintf(str_buffer, "\r\n*** SUCCESS! Module in u-connectXpress AT mode ***\r\n");
            HAL_UART_Transmit(getDebugPort(),(uint8_t*)str_buffer,strlen(str_buffer),5000);
        }
    } else {
        sprintf(str_buffer, "No response - trying AT command anyway\r\n");
        HAL_UART_Transmit(getDebugPort(),(uint8_t*)str_buffer,strlen(str_buffer),5000);
    }
}
void Nora_command(const char *cmd) {
    uint8_t rx[256];
    uint8_t dummy[256];

    // FLUSH any stale data in RX buffer BEFORE sending
    HAL_UART_Receive(getNoraPort(), dummy, sizeof(dummy), 10);

    // Send command
    HAL_UART_Transmit(getNoraPort(), (uint8_t*)cmd, strlen(cmd), 5000);

    // Wait longer for module to process and respond
    HAL_Delay(1000);  // Increased from 300ms

    // NOW receive response
    int rec = HAL_UART_Receive(getNoraPort(), rx, sizeof(rx), 2000);

    char str_buffer2[500];
    sprintf(str_buffer2,"\nRead Out from %s %d characters: \r\n",cmd,rec);
    HAL_UART_Transmit(getDebugPort(), str_buffer2, strlen(str_buffer2), 5000);
    HAL_UART_Transmit(getDebugPort(), rx, rec, 500);
    debug_print_hex(rx, rec);  // Uncomment this to see hex
}
//
//void Nora_startup(){
//	  uint8_t rx[256];
//	  HAL_GPIO_WritePin(Switch1_GPIO_Port, Switch1_Pin, GPIO_PIN_SET);
//	  HAL_GPIO_WritePin(Switch2_GPIO_Port, Switch2_Pin, GPIO_PIN_SET);
//	  HAL_GPIO_WritePin(GPIO_J9_GPIO_Port, GPIO_J9_Pin, GPIO_PIN_RESET);
//
//	  HAL_Delay(20);
////	  // Reset pulse (required for boot-mode latching)
//	  HAL_GPIO_WritePin(NReset_GPIO_Port, NReset_Pin, GPIO_PIN_RESET);
//	  HAL_Delay(50);
//	  HAL_GPIO_WritePin(NReset_GPIO_Port, NReset_Pin, GPIO_PIN_SET);
//
//	  // Wait for module to boot the AT firmware
//	  HAL_Delay(2000);
//
//	    // Read any boot message
//
//	  int rec = HAL_UART_Receive(getNoraPort(), rx, sizeof(rx), 5000);
////	  int rec = Nora_read_all(rx, sizeof(rx), 500);
////	  HAL_UART_Transmit(getDebugPort(), rx, rec, 200);
////	  debug_print_hex(rx,rec);
//
////	  HAL_GPIO_WritePin(NReset_GPIO_Port, NReset_Pin, GPIO_PIN_RESET);
////	  HAL_Delay(10);
////	  HAL_GPIO_WritePin(NReset_GPIO_Port, NReset_Pin, GPIO_PIN_SET);
//
////	  HAL_GPIO_WritePin(GPIO_J9_GPIO_Port, GPIO_J9_Pin, GPIO_PIN_RESET);
//
//	  // Boot into AT mode
//	  //	  HAL_GPIO_WritePin(NReset_GPIO_Port, NReset_Pin, GPIO_PIN_RESET);
//	  //	  HAL_Delay(10);
//	  //	  HAL_GPIO_WritePin(NReset_GPIO_Port, NReset_Pin, GPIO_PIN_SET);
//
//	//  HAL_Delay(10);
//	//  HAL_GPIO_WritePin(NReset_GPIO_Port, NReset_Pin, GPIO_PIN_SET);
////	  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
////	  HAL_Delay(2000);
////	//  HAL_GPIO_WritePin(GPIO_J9_GPIO_Port, GPIO_J9_Pin, GPIO_PIN_SET);
////
////	  HAL_UART_Receive(getNoraPort(), rx, sizeof(rx), 1000);
//	  HAL_UART_Transmit(getDebugPort(), rx, rec, 500);
//	  char str_buffer3[500];
//	  sprintf(str_buffer3,"\r\n _______________________ \r\n NEW RUN \r\n _______________________ \r\n\0");
//	  HAL_UART_Transmit(getDebugPort(),(uint8_t*)str_buffer3,strlen(str_buffer3),5000);
//}

//void Nora_command(const char *cmd) {
//	  uint8_t rx[256];
//	  HAL_UART_Transmit(getNoraPort(), (uint8_t*)cmd, strlen(cmd), 5000);
//	  HAL_Delay(300);
//	  // Receive response
//	  int rec = HAL_UART_Receive(getNoraPort(), rx, sizeof(rx), 5000);
//
//	  char str_buffer2[500];
//	  sprintf(str_buffer2,"\nRead Out from %s %d characters: \r\n\0",cmd,rec);
//	  HAL_UART_Transmit(getDebugPort(), str_buffer2, strlen(str_buffer2), 5000);
//	  HAL_UART_Transmit(getDebugPort(), rx, rec, 500);
////	  debug_print_hex(rx,rec);
//}
