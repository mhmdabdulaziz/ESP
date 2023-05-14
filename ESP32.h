/***
 *      Conure Tracker
 *    ESP32 Module Driver
 *      26th Apr 2023
 *  Author : Mohamed Abdulaziz
*/

#ifndef ESP32_H_
#define ESP32_H_

//Includes=============================//
#include "stm32f4xx_hal.h"
#include "powerControl.h"
//Macros===============================//

//Stractures===========================//


//Functions Decleration================//

void ESP_WiFi_Init(char *ssid, char *password);

char* Server_Connect(char *ip, char *port) ;

void Send_Data(char *data);

void Server_Disconnect(void);

uint8_t checkForReceivedData(char* expectedResponse);

void ESP_BLE_Init(void);
void ESP_BLE_ScanDevices(void);

uint8_t Look_for(char* response);

#endif /* ESP32_H_ */
