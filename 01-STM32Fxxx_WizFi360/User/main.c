/**
 * Keil project example for WizFi360 module
 *
 * Before you start, select your target, on the right of the "Load" button
 *
 * In this example, when you press button on discovery/nucleo board,
 * ESP goes to stm32f4-discovery and reads content of web page.
 *
 * @author    Tilen Majerle
 * @email     tilen@majerle.eu
 * @website   http://stm32f4-discovery.com
 * @ide       Keil uVision 5
 * @conf      PLL parameters are set in "Options for Target" -> "C/C++" -> "Defines"
 * @packs     STM32F4xx/STM32F7xx Keil packs are requred with HAL driver support
 * @stdperiph STM32F4xx/STM32F7xx HAL drivers required
 */
/* Include core modules */
#include "stm32fxxx_hal.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32_disco.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_usart.h"
#include "WizFi360.h"

/* WizFi360 working structure */
WizFi360_t WizFi360;

int main(void) {
	uint8_t sock;
	char tmp[20];
	WizFi360_Connection_t* Connection;
	
	/* Init system */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* Init button */
	TM_DISCO_ButtonInit();
	
	/* Init delay */
	TM_DELAY_Init();
	
	/* Init debug USART */
	TM_USART_Init(USART2, TM_USART_PinsPack_1, 115200);
	
	/* Display message */
	printf("WizFi360 AT commands parser\r\n");
	
	/* Init ESP module */
	while (WizFi360_Init(&WizFi360, 115200) != ESP_OK) {
		printf("Problems with initializing module!\r\n");
	}
	
	/* Set mode to STA+AP */
	while (WizFi360_SetMode(&WizFi360, WizFi360_Mode_STA_AP) != ESP_OK);
	
	/* Enable server on port 80 */
	while (WizFi360_ServerEnable(&WizFi360, 80) != ESP_OK);
	
	/* Module is connected OK */
	printf("Initialization finished!\r\n");
	
	/* Disconnect from wifi if connected */
	WizFi360_WifiDisconnect(&WizFi360);
	
	/* Get a list of all stations  */
	WizFi360_ListWifiStations(&WizFi360);
	
	/* Wait till finishes */
	WizFi360_WaitReady(&WizFi360);
	
	/* Connect to wifi and save settings */
	#if 1
	WizFi360_WifiConnect(&WizFi360, "YourSSID1", "YourPassword1");
	#else
	WizFi360_WifiConnect(&WizFi360, "YourSSID2", "YourPassword2");
	#endif
	
	/* Wait till finish */
	WizFi360_WaitReady(&WizFi360);
	
	/* Get connected devices */
	WizFi360_WifiGetConnected(&WizFi360);
	
	WizFi360_Update(&WizFi360);

	#if 1
	// taylor PC
	while (WizFi360_StartClientConnection(&WizFi360, "taylor-pc", "192.168.100.13", 5000, NULL));
	#else
	// becky PC
	while (WizFi360_StartClientConnection(&WizFi360, "becky-pc", "192.168.1.65", 5000, NULL));
	#endif
	sock = WizFi360.StartConnectionSent;
	WizFi360_WaitReady(&WizFi360);
	
	sprintf(Connection->Data,"abcdefghijklmnopqrstuvwxyz\r\n");
	WizFi360_RequestSendData(&WizFi360, Connection);


	#if 0
	// for UDP

	while (WizFi360_StartUDPConnection(&WizFi360, "becky-pc", "192.168.1.65", 5000, NULL));
	sock = WizFi360.StartConnectionSent;
	WizFi360_WaitReady(&WizFi360);
	sprintf(Connection->Data,"abcdefghijklmnopqrstuvwxyz\r\n");
	WizFi360_RequestSendData(&WizFi360, Connection);
	#endif

	while (1) {		
		if( WizFi360.Connection[sock].CallDataReceived == 1){
			WizFi360.Connection[sock].CallDataReceived =0;
			WizFi360_RequestSendData(&WizFi360, &WizFi360.Connection[sock] );
		}
		WizFi360_Update(&WizFi360);
	}
}

/* 1ms handler */
void TM_DELAY_1msHandler() {	
	/* Update WizFi360 library time for 1 ms */
	WizFi360_TimeUpdate(&WizFi360, 1);
}

/* printf handler */
int fputc(int ch, FILE* fil) {
	/* Send over debug USART */
	TM_USART_Putc(USART2, ch);
	
	/* Return OK */
	return ch;
}

/* Called when ready string detected */
void WizFi360_Callback_DeviceReady(WizFi360_t* WizFi360) {
	printf("Device is ready\r\n");
}

/* Called when watchdog reset on WizFi360 is detected */
void WizFi360_Callback_WatchdogReset(WizFi360_t* WizFi360) {
	printf("Watchdog reset detected!\r\n");
}

/* Called when we are disconnected from WIFI */
void WizFi360_Callback_WifiDisconnected(WizFi360_t* WizFi360) {
	printf("Wifi is disconnected!\r\n");
}

void WizFi360_Callback_WifiConnected(WizFi360_t* WizFi360) {
	printf("Wifi is connected!\r\n");
}

void WizFi360_Callback_WifiConnectFailed(WizFi360_t* WizFi360) {
	printf("Connection to wifi network has failed. Reason %d\r\n", WizFi360->WifiConnectError);
}

void WizFi360_Callback_WifiGotIP(WizFi360_t* WizFi360) {
	printf("Wifi got an IP address\r\n");
	
	/* Read that IP from module */
	printf("Grabbing IP status: %d\r\n", WizFi360_GetSTAIP(WizFi360));
}

void WizFi360_Callback_WifiIPSet(WizFi360_t* WizFi360) {
	/* We have STA IP set (IP set by router we are connected to) */
	printf("We have valid IP address: %d.%d.%d.%d\r\n", WizFi360->STAIP[0], WizFi360->STAIP[1], WizFi360->STAIP[2], WizFi360->STAIP[3]);
}

void WizFi360_Callback_DHCPTimeout(WizFi360_t* WizFi360) {
	printf("DHCP timeout!\r\n");
}

void WizFi360_Callback_WifiDetected(WizFi360_t* WizFi360, WizFi360_APs_t* WizFi360_AP) {
	uint8_t i = 0;
	
	/* Print number of detected stations */
	printf("We have detected %d AP stations\r\n", WizFi360_AP->Count);
	
	/* Print each AP */
	for (i = 0; i < WizFi360_AP->Count; i++) {
		/* Print SSID for each AP */
		printf("%2d: %s\r\n", i, WizFi360_AP->AP[i].SSID);
	}
}

/************************************/
/*         CLIENT CALLBACKS         */
/************************************/
void WizFi360_Callback_ClientConnectionConnected(WizFi360_t* WizFi360, WizFi360_Connection_t* Connection) {
	/* We are connected to external server */
	printf("Client connected to server! Connection number: %s\r\n", Connection->Name);
	
	/* We are connected to server, request to sent header data to server */
//	WizFi360_RequestSendData(WizFi360, Connection);
}

/* Called when client connection fails to server */
void WizFi360_Callback_ClientConnectionError(WizFi360_t* WizFi360, WizFi360_Connection_t* Connection) {
	/* Fail with connection to server */
	printf("An error occured when trying to connect on connection: %d\r\n", Connection->Number);
}

/* Called when data are ready to be sent to server */
// uint16_t WizFi360_Callback_ClientConnectionSendData(WizFi360_t* WizFi360, WizFi360_Connection_t* Connection, char* Buffer, uint16_t max_buffer_size) {
// 	/* Format data to sent to server */
// 	#if 1
// 	sprintf(Buffer, "Hello, from WizFi360\r\n");
// 	#else
// 	sprintf(Buffer, "GET / HTTP/1.1\r\n");
// 	strcat(Buffer, "Host: stm32f4-discovery.com\r\n");
// 	strcat(Buffer, "Connection: close\r\n");
// 	strcat(Buffer, "\r\n");
// 	#endif

// 	/* Return length of buffer */
// 	return strlen(Buffer);
// }

/* Called when data are send successfully */
void WizFi360_Callback_ClientConnectionDataSent(WizFi360_t* WizFi360, WizFi360_Connection_t* Connection) {
	printf("Data successfully sent as client!\r\n");
}

/* Called when error returned trying to sent data */
void WizFi360_Callback_ClientConnectionDataSentError(WizFi360_t* WizFi360, WizFi360_Connection_t* Connection) {
	printf("Error while sending data on connection %d!\r\n", Connection->Number);
}

uint32_t time = 0;
void WizFi360_Callback_ClientConnectionDataReceived(WizFi360_t* WizFi360, WizFi360_Connection_t* Connection, char* Buffer) {
	/* Data received from server back to client */
	printf("Data received from server on connection: %s; Number of bytes received: %d; %d / %d;\r\n",
		Connection->Name,
		Connection->BytesReceived,
		Connection->TotalBytesReceived,
		Connection->ContentLength
	);
	
	/* Print message when first packet */
	if (Connection->FirstPacket) {
		/* Start counting time */
		time = TM_DELAY_Time();
		
		/* Print first message */
		printf("This is first packet received. Content length on this connection is: %d\r\n", Connection->ContentLength);
	}
}

/* Called when connection is closed */
void WizFi360_Callback_ClientConnectionClosed(WizFi360_t* WizFi360, WizFi360_Connection_t* Connection) {
	printf("Client connection closed, connection: %d; Total bytes received: %d; Content-Length header: %d\r\n",
		Connection->Number, Connection->TotalBytesReceived, Connection->ContentLength
	);
	
	/* Calculate time */
	time = TM_DELAY_Time() - time;
	
	/* Print time we need to get data back from server */
	printf("Time for data: %u ms; speed: %d kb/s\r\n", time, Connection->TotalBytesReceived / time);
}

/* Called when timeout is reached on connection to server */
void WizFi360_Callback_ClientConnectionTimeout(WizFi360_t* WizFi360, WizFi360_Connection_t* Connection) {
	printf("Timeout reached on connection: %d\r\n", Connection->Number);
}
