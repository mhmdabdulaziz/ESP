/***
 *      Conure Tracker
 *    ESP32 Module Driver
 *      26th Apr 2023
 *  Author : Mohamed Abdulaziz
*/

//Includes==============================//
#include "ESP32.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

char Mode_Selection[4];
//Functions Implementation==============//
	//WIFI Functions
void ESP_WiFi_Init(char *ssid, char *password) {
	char cred_buffer[100];

  	//ESP Init
	HAL_UART_Transmit(&huart1, "AT+RESTORE\r\n", strlen("AT+RESTORE\r\n"), 1000);
	while(!(Look_for("ready") || Look_for("OK") ));

	HAL_UART_Transmit(&huart1, "AT+RST\r\n", strlen("AT+RST\r\n"), 1000);
	while(!(Look_for("ready"))|| Look_for("OK"));

	HAL_UART_Transmit(&huart1, "AT+CWMODE=1\r\n", strlen("AT+CWMODE=1\r\n"), 1000);
	while(!(Look_for("OK")));

	sprintf(cred_buffer, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
	HAL_UART_Transmit(&huart1, cred_buffer, sizeof(cred_buffer), 1000);
	while(!(Look_for("OK") || Look_for("WIFI CONNECTED") ));

	HAL_UART_Transmit(&huart1, "AT+CIPMUX=0\r\n",  strlen("AT+CIPMUX=0\r\n"), 1000);
	while(!(Look_for("OK")));

	HAL_UART_Transmit(&huart1, "AT+CIPRECVMODE=0\r\n", strlen("AT+CIPRECVMODE=0\r\n"), 1000);
	while(!(Look_for("OK")));
}

char* Server_Connect(char *ip, char *port) {
	char net_buffer[100];

	//Connect to a server
	sprintf(net_buffer, "AT+CIPSTART=\"TCP\",\"%s\",%s,1\r\n", ip, port);
	HAL_UART_Transmit(&huart1, net_buffer, sizeof(net_buffer), 1000);
	while(!(checkForReceivedData("IPD")));

	return Mode_Selection;
}

void Server_Disconnect(void) {
	HAL_UART_Transmit(&huart1, "AT+CIPCLOSE\r\n", strlen("AT+CIPCLOSE\r\n"), 1000);
	while(!(Look_for("OK")|| Look_for("CLOSED")));
}

void Send_Data(char *data){
	char data_buffer[100];

	sprintf(data_buffer, "AT+CIPSEND=%d\r\n", strlen(data));
	HAL_UART_Transmit(&huart1, data_buffer, sizeof(data_buffer), 1000);
	while(!(Look_for(">") || Look_for("OK") ));

	HAL_UART_Transmit(&huart1, data, strlen(data), 1000);
	while(!(Look_for("SEND OK")));
}

uint8_t checkForReceivedData(char* expectedResponse) {
    char receivedData[400] = {}; // Initialize the receivedData array
    HAL_UART_Receive(&huart1, receivedData, sizeof(receivedData), 2000);

    // Check if the expected response is present in the received data
    if (strstr(receivedData, expectedResponse) != NULL) {
        const char* colonPosition = strchr(receivedData, ':');
        if (colonPosition != NULL) {
            const char* incomingData = colonPosition + 1;
            strcpy(Mode_Selection,incomingData);
        } else {
            HAL_UART_Transmit(&huart2, "\r\nNo data after colon\r\n", strlen("\r\nNo data after colon\r\n"), 1000);
        }
        return 1;
    } else {
        HAL_UART_Transmit(&huart2, "\r\nExpected response not received\r\n", strlen("\r\nExpected response not received\r\n"), 1000);
        return 0;
    }
}

	//BLE Functions
void ESP_BLE_Init(void) {
  	//BLE Init
	HAL_UART_Transmit(&huart1, "AT+BLEINIT=2\r\n", strlen("AT+BLEINIT=2\r\n"), 1000);
	while(!(Look_for("OK")));

	//Create service
	HAL_UART_Transmit(&huart1, "AT+BLEGATTSSRVCRE\r\n", strlen("AT+BLEGATTSSRVCRE\r\n"), 1000);
	while(!(Look_for("OK")));

	//Start the service
	HAL_UART_Transmit(&huart1, "AT+BLEGATTSSRVSTART\r\n", strlen("AT+BLEGATTSSRVSTART\r\n"), 1000);
	while(!(Look_for("OK")));

	//Start Advertising
	HAL_UART_Transmit(&huart1, "AT+BLEADVSTART\r\n", strlen("AT+BLEADVSTART\r\n"), 1000);
	while(!(Look_for("OK")));
}

void ESP_BLE_ScanDevices(void) {
	//Scan for nearby devices
	HAL_UART_Transmit(&huart1, "AT+BLESCAN=1,3\r\n", strlen("AT+BLESCAN=1,3\r\n"), 1000);
	while(!(Look_for("OK")));
}

	//Utilities
uint8_t Look_for(char* response) {
	char data[400] = {};
    HAL_UART_Receive(&huart1, data, sizeof(data), 2000);
    if(strstr(data, response)!= NULL) {
    	HAL_UART_Transmit(&huart2, "\r\n", strlen("\r\n"), 100);
    	HAL_UART_Transmit(&huart2, data, sizeof(data), 1000);
    	HAL_UART_Transmit(&huart2, "\r\n", strlen("\r\n"), 100);
        return 1;
    } else {
        return 0;
    }
    HAL_Delay(100);
}
