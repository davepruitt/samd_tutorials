#include <atmel_start.h>
#include <stdlib.h>
#include <string.h>
#include "winc_init.h"

#define WIFI_SSID		"yourNetworkID"
#define WIFI_PASSWORD	"yourWifiPassword"

void wifi_cb(uint8_t u8MsgType, void *pvMsg)
{
	switch(u8MsgType)
	{
		case M2M_WIFI_RESP_CON_STATE_CHANGED:
		{
			tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *) pvMsg;
			if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED)
			{
				
			}
			else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED)
			{
				
			}
		}
	}
}

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	tstrNetworkId networkid;
	tstrAuthPsk authinfo;
	
	tstrWifiInitParam param;
	memset((uint8_t *)&param, 0, sizeof(tstrWifiInitParam));
	param.pfAppWifiCb = wifi_cb;
	wifi_init(&param);
	
	memset((uint8_t *) &networkid, 0, sizeof(tstrNetworkId));
	memset((uint8_t *) &authinfo, 0, sizeof(tstrAuthPsk));
	
	networkid.enuChannel = M2M_WIFI_CH_ALL;
	networkid.pu8Ssid = (uint8 *) WIFI_SSID;
	networkid.u8SsidLen = strlen(WIFI_SSID);
	authinfo.pu8Psk = NULL;
	authinfo.pu8Passphrase = (uint8 *) WIFI_PASSWORD;
	authinfo.u8PassphraseLen = strlen(WIFI_PASSWORD);
	
	int8_t ret = m2m_wifi_connect_psk(WIFI_CRED_DONTSAVE, &networkid, &authinfo);

	/* Replace with your application code */
	while (1) 
	{
		while(m2m_wifi_handle_events(NULL) != M2M_SUCCESS)
		{
			//empty
		}
	}
}
