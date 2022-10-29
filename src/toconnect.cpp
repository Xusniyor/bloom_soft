/*
WiFi Events
0  SYSTEM_EVENT_WIFI_READY               < ESP32 WiFi ready
1  SYSTEM_EVENT_SCAN_DONE                < ESP32 finish scanning AP
2  SYSTEM_EVENT_STA_START                < ESP32 station start
3  SYSTEM_EVENT_STA_STOP                 < ESP32 station stop
4  SYSTEM_EVENT_STA_CONNECTED            < ESP32 station connected to AP
5  SYSTEM_EVENT_STA_DISCONNECTED         < ESP32 station disconnected from AP
6  SYSTEM_EVENT_STA_AUTHMODE_CHANGE      < the auth mode of AP connected by ESP32 station changed
7  SYSTEM_EVENT_STA_GOT_IP               < ESP32 station got IP from connected AP
8  SYSTEM_EVENT_STA_LOST_IP              < ESP32 station lost IP and the IP is reset to 0
9  SYSTEM_EVENT_STA_WPS_ER_SUCCESS       < ESP32 station wps succeeds in enrollee mode
10 SYSTEM_EVENT_STA_WPS_ER_FAILED        < ESP32 station wps fails in enrollee mode
11 SYSTEM_EVENT_STA_WPS_ER_TIMEOUT       < ESP32 station wps timeout in enrollee mode
12 SYSTEM_EVENT_STA_WPS_ER_PIN           < ESP32 station wps pin code in enrollee mode
13 SYSTEM_EVENT_AP_START                 < ESP32 soft-AP start
14 SYSTEM_EVENT_AP_STOP                  < ESP32 soft-AP stop
15 SYSTEM_EVENT_AP_STACONNECTED          < a station connected to ESP32 soft-AP
16 SYSTEM_EVENT_AP_STADISCONNECTED       < a station disconnected from ESP32 soft-AP
17 SYSTEM_EVENT_AP_STAIPASSIGNED         < ESP32 soft-AP assign an IP to a connected station
18 SYSTEM_EVENT_AP_PROBEREQRECVED        < Receive probe request packet in soft-AP interface
19 SYSTEM_EVENT_GOT_IP6                  < ESP32 station or ap or ethernet interface v6IP addr is preferred
20 SYSTEM_EVENT_ETH_START                < ESP32 ethernet start
21 SYSTEM_EVENT_ETH_STOP                 < ESP32 ethernet stop
22 SYSTEM_EVENT_ETH_CONNECTED            < ESP32 ethernet phy link up
23 SYSTEM_EVENT_ETH_DISCONNECTED         < ESP32 ethernet phy link down
24 SYSTEM_EVENT_ETH_GOT_IP               < ESP32 ethernet got IP from connected AP
25 SYSTEM_EVENT_MAX
*/
#include <Arduino.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <toconnect.h>
#include <toflash.h>

WiFiServer server(80);
IPAddress  ip;

wifi_def     s_wifi_ap;  // ulanish nuqtasi
wifi_def     s_wifi_sta; // ulanish wifi
networks_def s_scan[64]; // maksimal 32 ta network

char user_login   [MAX_LENGTH_FLASH];
char user_password[MAX_LENGTH_FLASH];
int  connect_mode;
unsigned int networks_found;

extern EEPROMClass  f_ssid_ap;
extern EEPROMClass  f_password_ap;
extern EEPROMClass  f_ssid_sta;
extern EEPROMClass  f_password_sta;

String hostname = "BLOOM";


// Set your Static IP address
// IPAddress local_IP(192, 168, 1, 184);
// // Set your Gateway IP address
// IPAddress gateway(192, 168, 1, 1);
// IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional



void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch (event)
  {
    case SYSTEM_EVENT_WIFI_READY: 
      Serial.println("WiFi interface ready");
      break;
    case SYSTEM_EVENT_SCAN_DONE:
      Serial.println("Completed scan for access points");
      break;
    case SYSTEM_EVENT_STA_START:
      Serial.println("WiFi client started");
      break;
    case SYSTEM_EVENT_STA_STOP:
      Serial.println("WiFi clients stopped");
      break;
    case SYSTEM_EVENT_STA_CONNECTED:
      Serial.println("Connected to access point");
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("Disconnected from WiFi access point");
      WiFi.begin((const char *)s_wifi_sta.ssid, (const char *)s_wifi_sta.password);
      break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
      Serial.println("Authentication mode of access point has changed");
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.print("Obtained IP address: ");
      Serial.println(WiFi.localIP());
      break;
    case SYSTEM_EVENT_STA_LOST_IP:
      Serial.println("Lost IP address and IP address is reset to 0");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
      Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
      Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
      Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
      Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
      break;
    case SYSTEM_EVENT_AP_START:
      Serial.println("WiFi access point started");
      break;
    case SYSTEM_EVENT_AP_STOP:
      Serial.println("WiFi access point  stopped");
      break;
    case SYSTEM_EVENT_AP_STACONNECTED:
      Serial.println("Client connected");
      break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
      Serial.println("Client disconnected");
      break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
      Serial.println("Assigned IP address to client");
      break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:
      Serial.println("Received probe request");
      break;
    case SYSTEM_EVENT_GOT_IP6:
      Serial.println("IPv6 is preferred");
      break;
    case SYSTEM_EVENT_ETH_START:
      Serial.println("Ethernet started");
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("Ethernet stopped");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("Ethernet connected");
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("Ethernet disconnected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.println("Obtained IP address");
      break;
    default: break;
    }
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
}
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("Connected to AP successfully!");
}
// void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
//   Serial.println("WiFi connected");
//   Serial.println("IP address: ");
//   Serial.println(WiFi.localIP());
// }
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.begin((const char *)s_wifi_sta.ssid, (const char *)s_wifi_sta.password);
}

void toConnect_ScanNetworks(void)
{
    Serial.println("networks scan start");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    networks_found = WiFi.scanNetworks();
    if (networks_found != 0)
    {
        Serial.print  (networks_found);
        Serial.println(" networks found");
        for (int i_index = 0; i_index < networks_found; i_index++)
        {
            WiFi.SSID(i_index).toCharArray(s_scan[i_index].ssid, sizeof(s_scan[i_index].ssid));
            s_scan[i_index].rssi      =  WiFi.RSSI(i_index);
            s_scan[i_index].auth_open = (WiFi.encryptionType(i_index) == WIFI_AUTH_OPEN) ? true : false;
            Serial.print(i_index + 1); Serial.print(": "); Serial.print(s_scan[i_index].ssid); Serial.print(" ("); Serial.print(s_scan[i_index].rssi); Serial.print(") "); Serial.println(s_scan[i_index].auth_open ? "OPEN" : "CLOSE");
        }
    }
    else
    {
        Serial.println("no networks found");
    }
}
void toConnect_Connect(void)
{
    WiFi.disconnect(true);
    // delay(1000);
    // WiFi.onEvent(WiFiEvent);
    // WiFi.onEvent(WiFiGotIP, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
    // WiFiEventId_t eventID = WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info){
    //     Serial.print("WiFi lost connection. Reason: ");
    //     Serial.println(info.disconnected.reason);
    // }, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
    // Serial.print("WiFi Event ID: ");
    // Serial.println(eventID);
    // WiFi.reconnect();
    // WiFi.disconnect();
    // WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
    // WiFi.onEvent(WiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);
    // WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
    // WiFi.begin((const char *)s_wifi_sta.ssid, (const char *)s_wifi_sta.password);
    // return;

    // if (!WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, primaryDNS, secondaryDNS))
    // {
    //   Serial.println("STA Failed to configure");
    // }
  //connect_mode = WIFI_AP;
    if (connect_mode == WIFI_AP)
    {
        Serial.println("Connect mode WIFI_AP");
        WiFi.mode(WIFI_AP);
        WiFi.softAP((const char *)s_wifi_ap.ssid, (const char *)s_wifi_ap.password);
        server.begin();
        ip = WiFi.softAPIP();
        Serial.print("IP Address: "); Serial.println(ip);
    }
    else if (connect_mode == WIFI_STA)
    {
        Serial.println("Connect mode WIFI_STA");
        WiFi.mode(WIFI_STA);
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
        WiFi.setHostname(hostname.c_str());
        WiFi.begin((const char *)s_wifi_sta.ssid, (const char *)s_wifi_sta.password);
        unsigned long time_out = millis();
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(1000); Serial.print(".");
            if (millis() - time_out > 5000)
            {
                Serial.println();
                Serial.println("Connect mode WIFI_STA time out");
                Serial.println("Connect mode WIFI_AP");
                connect_mode = WIFI_AP;
                WiFi.mode(WIFI_AP);
                WiFi.softAP((const char *)s_wifi_ap.ssid, (const char *)s_wifi_ap.password);
                server.begin();
                ip = WiFi.softAPIP();
                Serial.println();
                Serial.print("IP Address: "); Serial.println(ip);
                return;
            }
        }
        server.begin();
        // ip = WiFi.localIP();
        Serial.println();
        // Serial.print("IP Address: "); Serial.println(ip);
        Serial.print("Local IP:    ");
        Serial.println(WiFi.localIP());
        Serial.print("Subnet Mask: ");
        Serial.println(WiFi.subnetMask());
        Serial.print("Gateway IP:  ");
        Serial.println(WiFi.gatewayIP());
        Serial.print("DNS 1:       ");
        Serial.println(WiFi.dnsIP(0));
        Serial.print("DNS 2:       ");
        Serial.println(WiFi.dnsIP(1));
    }
    else
    {
        Serial.println("Connect mode error");
        connect_mode = WIFI_AP;
        EEPROM.write(ADDRESS_CONNECT_MODE, WIFI_AP);
        delay(1000);
        ESP.restart();
    }
}
void toConnect_NewSta(const char * a_ssid, const char * a_password)
{
    memset(s_wifi_sta.ssid,      '\0', sizeof(s_wifi_sta.ssid));
    memset(s_wifi_sta.password,  '\0', sizeof(s_wifi_sta.password));
    strcpy(s_wifi_sta.ssid,      a_ssid);
    strcpy(s_wifi_sta.password,  a_password);
    f_ssid_sta.writeString    (0, s_wifi_sta.ssid);     f_ssid_sta.commit();
    f_password_sta.writeString(0, s_wifi_sta.password); f_password_sta.commit();
}
void toConnect_NewAp(const char * a_ssid, const char * a_password)
{
    memset(s_wifi_ap.ssid,      '\0', sizeof(s_wifi_ap.ssid));
    memset(s_wifi_ap.password,  '\0', sizeof(s_wifi_ap.password));
    strcpy(s_wifi_ap.ssid,      a_ssid);
    strcpy(s_wifi_ap.password,  a_password);
    f_ssid_ap.writeString    (0, s_wifi_ap.ssid);     f_ssid_ap.commit();
    f_password_ap.writeString(0, s_wifi_ap.password); f_password_ap.commit();
}