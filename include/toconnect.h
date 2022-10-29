#ifndef __TOCONNECT_H_
#define __TOCONNECT_H_
#include <toflash.h>
typedef struct
{
  char ssid    [MAX_LENGTH_FLASH];
  char password[MAX_LENGTH_FLASH];
} wifi_def;
typedef struct
{
  bool auth_open;
  int  rssi;
  char ssid  [MAX_LENGTH_FLASH];
} networks_def;

void toConnect_ScanNetworks(void);
void toConnect_Connect(void);
void toConnect_NewSta(const char * a_ssid, const char * a_password);
void toConnect_NewAp(const char * a_ssid, const char * a_password);

#endif /*__TOCONNECT_H_*/