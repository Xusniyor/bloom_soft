#include <ArduinoJson.h>
#include <Arduino.h>
#include <ESP32Time.h>
#include <time.h>
#include <otadrive_esp.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <toflash.h>
#include <totime.h>
#include <toio.h>
#include <toconnect.h>
#include <toconfig.h>
#include <tocmd.h>
#include <toupdate.h>
#include "esp_wifi.h"
#include <math.h>

void TaskFirst (void *pvParameters);
void TaskSecond(void *pvParameters);

ESP32Time  rtc;

extern EEPROMClass f_url_update;
extern EEPROMClass f_key_update;

extern WiFiServer server;

extern wifi_def s_wifi_ap;  // ulanish nuqtasi
extern wifi_def s_wifi_sta; // ulanish wifi

extern alarm_def s_alarm[MAX_ALARM_INDEX];
extern networks_def s_scan[20]; // maksimal 20 ta network


extern const int freq = 20000;
extern const int ledChannel = 0;
extern const int resolution = 8;


extern char device_name  [MAX_LENGTH_FLASH];
extern char user_login   [MAX_LENGTH_FLASH];
extern char user_password[MAX_LENGTH_FLASH];
extern char url_update[MAX_URL_KEY_LEN];
extern char key_update[MAX_URL_KEY_LEN];
extern unsigned int last_index_alarm;
extern unsigned int networks_found;
extern int connect_mode;
extern bool auto_update;

unsigned int time_until_alarm;
unsigned int vibration_test;
unsigned int vibration_stop;

unsigned long newTimeToSend = 0;

bool lastConnectTime = false;

int last_ap_sta_num;

int test_temp = 0;

int isActive   = 0;
bool isSetTime = false;

// bool oto_enable;
// bool oto_server_enable;

void setup(void) {
  toIO_Init();
  rtc.setTime(1637301540);
  toFlash_Init();
  toFlash_ToDefault();
  toFlash_ReadAll();
  toConnect_ScanNetworks();
  toConnect_Connect();
  xTaskCreatePinnedToCore(TaskFirst,  "Task First" , 0x2000, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(TaskSecond, "Task Second", 0x8000, NULL, 2, NULL, 1);
}
void loop(void) {

}
void TaskFirst (void *pvParameters)
{
  (void) pvParameters;
  unsigned char m_index;
  unsigned int n_vibration;
  unsigned int real_time_min;
  unsigned int fin_min_time = 2000;
  toUpdate_Init();
  for(;;)
  {
    if ((last_index_alarm >= 1)&&(last_index_alarm <= 10)&&(isSetTime == true))
    {
      fin_min_time = 2000;
      real_time_min = rtc.getEpoch() % 86400;
      for (m_index = 0; m_index <= (last_index_alarm-1); m_index++)
      {
        if (s_alarm[m_index].turn == true)
        {
          if (s_alarm[m_index].week[rtc.getDayofWeek()] == 1)
          {
            if ((real_time_min >= s_alarm[m_index].time) && (real_time_min <= (s_alarm[m_index].time + 120)))
            {
              if (s_alarm[m_index].lock == false)
              {
                s_alarm[m_index].lock = true;
                n_vibration = s_alarm[m_index].temp;
                isActive = m_index + 1;
                Serial.println("Budilnik topildi.");
              }
            }
            else
            {
              s_alarm[m_index].lock = false;
            }
            if (s_alarm[m_index].time > real_time_min)
            {
              fin_min_time = min(fin_min_time, (s_alarm[m_index].time - real_time_min));
            }
          }
        }
      }
    }
    //=============================
    if (time_until_alarm != fin_min_time)
    {
      time_until_alarm = fin_min_time;
    }
    //=============================
    if (vibration_test)
    {
      Serial.println("Test temp.");
      if (vibration_test == 1)
      {
        //vibration_test = 64;
      }
      else if (vibration_test == 3)
      {
        vibration_test = 127;
      }
      else if (vibration_test == 5)
      {
        vibration_test = 191;
      }
      else if ((vibration_test == 6) || (vibration_test == 7))
      {
        vibration_test = 255;
      }
      else
      {
        vibration_test = 0;
      }
      if (vibration_test == 1)
      {
        for(int dutyCycle = 0; dutyCycle <= 254; dutyCycle++)
        {
          ledcWrite(ledChannel, dutyCycle);
          vTaskDelay(10);
        }
        for(int dutyCycle = 254; dutyCycle >= 0; dutyCycle--)
        {
          ledcWrite(ledChannel, dutyCycle);   
          vTaskDelay(10);
        }
      }
      else
      {
        for(int dutyCycle = 0; dutyCycle <= vibration_test; dutyCycle++)
        {
          ledcWrite(ledChannel, dutyCycle);
          vTaskDelay(1);
        }
        vTaskDelay(500);
        for(int dutyCycle = vibration_test; dutyCycle >= 0; dutyCycle--)
        {
          ledcWrite(ledChannel, dutyCycle);   
          vTaskDelay(1);
        }
      }
      ledcWrite(ledChannel, 0);
      vibration_test = 0;
      isActive = 0;
    }
    //=============================
    if (n_vibration)
    {
      Serial.println("Budilnik chalindi.");
      if (n_vibration == 1)
      {
        //n_vibration = 64;
        //n_vibration == 254;
      }
      else if (n_vibration == 3)
      {
        n_vibration = 127;
      }
      else if (n_vibration == 5)
      {
        n_vibration = 191;
      }
      else if ((n_vibration == 6) || (n_vibration == 7))
      {
        n_vibration = 255;
      }
      else
      {
        n_vibration = 0;
      }
      //n_vibration = (n_vibration & 7) * 36;
      //n_vibration = 255;
      if (n_vibration == 1)
      {
        for (int times = 0; times < 100; times++)
        {
          if (lastConnectTime == true)
          {
            times = 0;
            lastConnectTime = false;
          }
          for(int dutyCycle = 0; dutyCycle <= 254; dutyCycle++)
          {
            if ((vibration_stop) || (!digitalRead(BUTTON_PIN)))
            {
              n_vibration = dutyCycle;
              //vibration_stop = 0;
              break;
            }
            ledcWrite(ledChannel, dutyCycle);
            vTaskDelay(10);
          }
          for (int i_wait = 0; i_wait < 1000; i_wait++)
          {
            if ((vibration_stop) || (!digitalRead(BUTTON_PIN)))
            {
              break;
            }
            vTaskDelay(1);
          }
          for(int dutyCycle = 254; dutyCycle >= 0; dutyCycle--)
          {
            ledcWrite(ledChannel, dutyCycle);   
            vTaskDelay(10);
          }
          ledcWrite(ledChannel, 0);
          for (int i_wait = 0; i_wait < 1000; i_wait++)
          {
            if ((vibration_stop) || (!digitalRead(BUTTON_PIN)))
            {
              break;
            }
            vTaskDelay(1);
          }
          if ((vibration_stop) || (!digitalRead(BUTTON_PIN)))
          {
            break;
          }
        }
      }
      else
      {
        for (int times = 0; times < 100; times++)
        {
          if (lastConnectTime == true)
          {
            times = 0;
            lastConnectTime = false;
          }
          for(int dutyCycle = 0; dutyCycle <= n_vibration; dutyCycle++)
          {
            if ((vibration_stop) || (!digitalRead(BUTTON_PIN)))
            {
              n_vibration = dutyCycle;
              //vibration_stop = 0;
              break;
            }
            ledcWrite(ledChannel, dutyCycle);
            vTaskDelay(1);
          }
          for (int i_wait = 0; i_wait < 1000; i_wait++)
          {
            if ((vibration_stop) || (!digitalRead(BUTTON_PIN)))
            {
              break;
            }
            vTaskDelay(1);
          }
          for(int dutyCycle = n_vibration; dutyCycle >= 0; dutyCycle--)
          {
            ledcWrite(ledChannel, dutyCycle);   
            vTaskDelay(1);
          }
          ledcWrite(ledChannel, 0);
          for (int i_wait = 0; i_wait < 1000; i_wait++)
          {
            if ((vibration_stop) || (!digitalRead(BUTTON_PIN)))
            {
              break;
            }
            vTaskDelay(1);
          }
          if ((vibration_stop) || (!digitalRead(BUTTON_PIN)))
          {
            break;
          }
        }
      }
      ledcWrite(ledChannel, 0);
      n_vibration = 0;
      vibration_stop = 0;
      isActive = 0;
    }
    //=============================
    if ((auto_update == true) && (connect_mode == WIFI_STA))
    {
      toUpdate_Update(time_until_alarm);
    }
    //=============================
    // if (vibration_test) 
    // {
    //   vibration_test = (vibration_test & 7) * 36;
    //   for(int dutyCycle = 0; dutyCycle <= vibration_test; dutyCycle++)
    //   {
    //     if (vibration_stop)
    //     {
    //       vibration_test = dutyCycle;
    //       vibration_stop = 0;
    //       break;
    //     }
    //     ledcWrite(ledChannel, dutyCycle);
    //     vTaskDelay(10);
    //   }
    //   for(int dutyCycle = vibration_test; dutyCycle >= 0; dutyCycle--)
    //   {
    //     ledcWrite(ledChannel, dutyCycle);   
    //     vTaskDelay(10);
    //   }
    //   vibration_test = 0;
    // }
    //=============================
    if (!digitalRead(BUTTON_PIN))
    {
      unsigned int tmout = 10;
      while (!digitalRead(BUTTON_PIN))
      {
        digitalWrite(LED_PIN, LOW);
        vTaskDelay(500);
        digitalWrite(LED_PIN, HIGH);
        vTaskDelay(500);
        if (!tmout--)
        {
          EEPROM.write(ADDRESS_DEFAULT, '8'); EEPROM.commit();
          digitalWrite(LED_PIN, LOW);
          while (!digitalRead(BUTTON_PIN));
          ESP.restart();
        }
      }
      Serial.println("Kinopka bosildi");
      n_vibration = 255;
      for(int dutyCycle = 0; dutyCycle <= n_vibration; dutyCycle++)
      {
        if (vibration_stop)
        {
          n_vibration = dutyCycle;
          vibration_stop = 0;
          break;
        }
        ledcWrite(ledChannel, dutyCycle);
        //vTaskDelay(10);
      }
      vTaskDelay(100);
      for(int dutyCycle = n_vibration; dutyCycle >= 0; dutyCycle--)
      {
        ledcWrite(ledChannel, dutyCycle);   
        //vTaskDelay(10);
      }
      n_vibration = 0;
    }
    //=============================
    if (connect_mode == WIFI_AP) 
    {
      digitalWrite(LED_PIN, HIGH);
      vTaskDelay(10);
      digitalWrite(LED_PIN, LOW);
      vTaskDelay(100);
    }
    else
    {
      digitalWrite(LED_PIN, LOW);
      vTaskDelay(10);
      digitalWrite(LED_PIN, HIGH);
      vTaskDelay(100);
    }
    if (rtc.getEpoch() != newTimeToSend)
    {
      newTimeToSend = rtc.getEpoch();
      Serial.print(rtc.getTime("%A, %B %d %Y %H:%M:%S : ")); Serial.println(rtc.getEpoch() % 86400);   // (String) returns time with specified format
    }
  }
}
void TaskSecond (void *pvParameters)
{
  (void) pvParameters;
  for(;;)
  {
    WiFiClient client = server.available();
    if (client)
    {
      Serial.print("Client connected with IP: ");
      Serial.println(client.remoteIP());
      StaticJsonDocument<1023> doc;
      StaticJsonDocument<4095> san;
      unsigned long time_out = millis();
      while (client.connected())
      {
        //lastConnectTime = rtc.getEpoch();
        lastConnectTime = true;
        if (client.available())
        {
          time_out = millis();
          DeserializationError jerror = deserializeJson(doc, client);
          if (jerror.code() == DeserializationError::Ok)
          {
            if (doc.containsKey("cmd"))
            {
              const char* u_cmd = doc["cmd"];
              if (strcmp(u_cmd, "ping") == 0)
              {
                san["devicename"] = user_login;
                san["deviceid"] = OTADRIVE.getChipId();
                serializeJson(san, client);
                client.stop();
              }
              else
              {
                if ((doc.containsKey("login"))&&(doc.containsKey("password")))
                {
                  const char* u_log  = doc["login"];
                  const char* u_pass = doc["password"];
                  if ((strcmp(u_log, user_login) == 0)&&(strcmp(u_pass, user_password) == 0))
                  {
                    //************************************************************************************************
                    if (strcmp(u_cmd, "settime") == 0)
                    {
                      if (doc.containsKey("time"))
                      {
                        isSetTime = true;
                        time_t  n_time = doc["time"];
                        rtc.setTime(n_time);
                        n_time = rtc.getEpoch();
                        san["time"] = n_time;
                        serializeJson(san, client);
                        client.stop();
                      }
                      else
                      {
                        san["error"] = "vaqt topilmadi";
                        serializeJson(san, client);
                        client.stop();
                      }
                    }
                    //***********************
                    else if (strcmp(u_cmd, "update") == 0)
                    {
                      bool qwertyui = false;
                      if (doc.containsKey("auto"))
                      {
                        auto_update = doc["auto"];
                        EEPROM.write(ADDRESS_AUTO_UPDATE, auto_update);
                        san["auto"] = auto_update;
                        qwertyui = true;
                      }
                      if (doc.containsKey("key"))
                      {
                        const char * u_update_key = doc["key"];
                        toUpdate_NewKey(u_update_key);
                        san["key"] = key_update;
                        qwertyui = true;
                      }
                      if (doc.containsKey("url"))
                      {
                        const char * u_update_url = doc["url"];
                        memset(url_update, '\0', sizeof(url_update));
                        strcpy(url_update, u_update_url);
                        f_url_update.writeString(0, url_update); f_url_update.commit();
                        san["url"] = url_update;
                        qwertyui = true;
                      }
                      if (qwertyui != true)
                      {
                        san["error"] = "xechnima topilamadi";
                      }
                      serializeJson(san, client);
                      client.stop();
                    }
                    //***********************
                    else if (strcmp(u_cmd, "gettime") == 0)
                    {
                      san["time"] = rtc.getEpoch();
                      serializeJson(san, client);
                      client.stop();
                    }
                    //***********************
                    else if (strcmp(u_cmd, "getstalist") == 0)
                    {
                      san["found"] = networks_found;
                      if (networks_found != 0)
                      {
                        for (int i_index = 0; i_index < networks_found; i_index++)
                        {
                          san["network"][i_index]["ssid"] = s_scan[i_index].ssid;
                          san["network"][i_index]["rssi"] = s_scan[i_index].rssi;
                          san["network"][i_index]["stat"] = s_scan[i_index].auth_open ? "open" : "close";
                        }
                      }
                      serializeJson(san, client);
                      client.stop();
                    }
                    //***********************
                    else if (strcmp(u_cmd, "updatenow") == 0)
                    {
                      san["ok"] = "update now";
                      serializeJson(san, client);
                      client.stop();
                      vTaskDelay(100);
                      toUpdate_Update(time_until_alarm);
                    }
                    //***********************
                    else if (strcmp(u_cmd, "reboot") == 0)
                    {
                      san["ok"] = "reboot";
                      serializeJson(san, client);
                      client.stop();
                      Serial.println("Reboot");
                      vTaskDelay(1000);
                      ESP.restart();
                    }
                    //***********************
                    else if (strcmp(u_cmd, "getversion") == 0)
                    {
                      san["version"] = DEFAULT_DEVICE_VERSION;
                      serializeJson(san, client);
                      client.stop();
                    }
                    //***********************
                    else if (strcmp(u_cmd, "testuser") == 0)
                    {
                      san["ok"] = "user";
                      serializeJson(san, client);
                      client.stop();
                    }
                    //***********************
                    else if (strcmp(u_cmd, "connectmode") == 0)
                    {
                      if (doc.containsKey("mode"))
                      {
                        const char * u_mode = doc["mode"];
                        if (strcmp(u_mode, "sta") == 0)
                        {
                          connect_mode = WIFI_STA;
                          san["mode"] = u_mode;
                        }
                        else if (strcmp(u_mode, "ap") == 0)
                        {
                          connect_mode = WIFI_AP;
                          san["mode"] = u_mode;
                        }
                        else
                        {
                          san["error"] = "mode notogri";
                        }
                        EEPROM.write(ADDRESS_CONNECT_MODE, connect_mode);
                        EEPROM.commit();
                        serializeJson(san, client);
                        client.stop();
                      }
                      else
                      {
                        san["error"] = "ulanish turi topilmadi";
                        serializeJson(san, client);
                        client.stop();
                      }
                    }
                    //***********************
                    else if (strcmp(u_cmd, "getconnectmode") == 0)
                    {
                      if (connect_mode == WIFI_STA)
                      {
                        san["mode"] = "sta";
                      }
                      else if (connect_mode == WIFI_AP)
                      {
                        san["mode"] = "ap";
                      }
                      else
                      {
                        san["error"] = "mode";
                      }
                      serializeJson(san, client);
                      client.stop();
                    }
                    //***********************
                    else if (strcmp(u_cmd, "newsta") == 0)
                    {
                      if ((doc.containsKey("ssidsta"))&&(doc.containsKey("passwordsta")))
                      {
                        const char * u_ssid = doc["ssidsta"];
                        const char * u_pass = doc["passwordsta"];
                        if ((strlen(u_ssid) >= 1)&&(strlen(u_pass) >= 8))
                        {
                          toConnect_NewSta(u_ssid, u_pass);
                          san["ssidsta"]     = s_wifi_sta.ssid;
                          san["passwordsta"] = s_wifi_sta.password;
                          serializeJson(san, client);
                          client.stop();
                        }
                        else
                        {
                          san["error"] = "nomi 1 dan kam yoki parol 8 dan kam";
                          serializeJson(san, client);
                          client.stop();
                        }
                      }
                      else
                      {
                        san["error"] = "nomi yoki paroli topilmadi";
                        serializeJson(san, client);
                        client.stop();
                      }
                    }
                    //***********************
                    else if (strcmp(u_cmd, "newstaandreboot") == 0)
                    {
                      if ((doc.containsKey("ssidsta"))&&(doc.containsKey("passwordsta")))
                      {
                        const char * u_ssid = doc["ssidsta"];
                        const char * u_pass = doc["passwordsta"];
                        if ((strlen(u_ssid) >= 1)&&(strlen(u_pass) >= 8))
                        {
                          toConnect_NewSta(u_ssid, u_pass);
                          EEPROM.write(ADDRESS_CONNECT_MODE, WIFI_STA);
                          EEPROM.commit();
                          connect_mode = WIFI_STA;
                          san["ssidsta"]     = s_wifi_sta.ssid;
                          san["passwordsta"] = s_wifi_sta.password;
                          serializeJson(san, client);
                          client.stop();
                          Serial.println("Reboot");
                          vTaskDelay(1000);
                          ESP.restart();
                        }
                        else
                        {
                          san["error"] = "nomi 1 dan kam yoki paroli 8 dan kam";
                          serializeJson(san, client);
                          client.stop();
                        }
                      }
                      else
                      {
                        san["error"] = "nomi yoki paroli topilmadi";
                        serializeJson(san, client);
                        client.stop();
                      }
                    }
                    //***********************
                    else if (strcmp(u_cmd, "newuser") == 0)
                    {
                      if ((doc.containsKey("newlogin"))&&(doc.containsKey("newpassword")))
                      {
                        const char * u_newlog  = doc["newlogin"];
                        const char * u_newpass = doc["newpassword"];
                        toCmd_NewUser(u_newlog, u_newpass);
                        san["newlogin"]    = user_login;
                        san["newpassword"] = user_password;
                        serializeJson(san, client);
                        client.stop();
                      }
                      else
                      {
                        san["error"] = "yangi login yoki parol topilmadi";
                        serializeJson(san, client);
                        client.stop();
                      }
                    }
                    //***********************
                    else if (strcmp(u_cmd, "newap") == 0)
                    {
                      if ((doc.containsKey("ssidap"))&&(doc.containsKey("passwordap")))
                      {
                        const char * u_ssid = doc["ssidap"];
                        const char * u_pass = doc["passwordap"];
                        if ((strlen(u_ssid) >= 1)&&(strlen(u_pass) >= 8))
                        {
                          toConnect_NewAp(u_ssid, u_pass);
                          san["ssidap"]     = s_wifi_ap.ssid;
                          san["passwordap"] = s_wifi_ap.password;
                          serializeJson(san, client);
                          client.stop();
                        }
                        else
                        {
                          san["error"] = "nomi 1 dan kam yoki paroli 8 dan kam";
                          serializeJson(san, client);
                          client.stop();
                        }
                      }
                      else
                      {
                        san["error"] = "yangi nomi yoki paroli topilmadi";
                        serializeJson(san, client);
                        client.stop();
                      }
                    }
                    //***********************
                    else if (strcmp(u_cmd, "getsta") == 0)
                    {
                      san["ssidsta"]     = s_wifi_sta.ssid;
                      san["passwordsta"] = s_wifi_sta.password;
                      serializeJson(san, client);
                      client.stop();
                    }
                    //***********************
                    else if (strcmp(u_cmd, "getap") == 0)
                    {
                      san["ssidap"]     = s_wifi_ap.ssid;
                      san["passwordap"] = s_wifi_ap.password;
                      serializeJson(san, client);
                      client.stop();
                    }
                    //***********************
                    else if (strcmp(u_cmd, "newapanduser") == 0)
                    {
                      if ((doc.containsKey("ssidap"))&&(doc.containsKey("passwordap")))
                      {
                        const char * u_ssid = doc["ssidap"];
                        const char * u_pass = doc["passwordap"];
                        if ((strlen(u_ssid) >= 1)&&(strlen(u_pass) >= 8))
                        {
                          toConnect_NewAp(u_ssid, u_pass);
                          toCmd_NewUser(u_ssid, u_pass);
                          san["ssidap"]     = s_wifi_ap.ssid;
                          san["passwordap"] = s_wifi_ap.password;
                          serializeJson(san, client);
                          client.stop();
                        }
                        else
                        {
                          san["error"] = "nomi 1 dan kam yoki paroli 8 dan kam";
                          serializeJson(san, client);
                          client.stop();
                        }
                      }
                      else
                      {
                        san["error"] = "nomi yoki paroli topilmadi";
                        serializeJson(san, client);
                        client.stop();
                      }
                    }
                    //***********************
                    else if (strcmp(u_cmd, "newalarm") == 0)
                    {
                      if ((doc.containsKey("mode"))&&(doc.containsKey("comment"))&&(doc.containsKey("time"))&&(doc.containsKey("week"))&&(doc.containsKey("temp"))&&(doc.containsKey("turn")))
                      {
                        if (last_index_alarm < (MAX_ALARM_INDEX-1))
                        {
                          last_index_alarm++;
                          EEPROM.write(ADDRESS_LAST_INDEX_ALARM, last_index_alarm); EEPROM.commit();
                          unsigned int n_index_alarm  = last_index_alarm - 1;
                          s_alarm[n_index_alarm].turn = doc["turn"];
                          s_alarm[n_index_alarm].temp = doc["temp"];
                          s_alarm[n_index_alarm].mode = doc["mode"];
                          const char* n_week          = doc["week"];
                          const char* n_comment       = doc["comment"];
                          time_t      n_time          = doc["time"];
                          toTime_SetComment(n_comment, n_index_alarm);
                          s_alarm[n_index_alarm].time = n_time;
                          sscanf(n_week, "*Y%1d,D%1d,S%1d,C%1d,P%1d,J%1d,W%1d#", &s_alarm[n_index_alarm].week[0], &s_alarm[n_index_alarm].week[1], &s_alarm[n_index_alarm].week[2], &s_alarm[n_index_alarm].week[3], &s_alarm[n_index_alarm].week[4], &s_alarm[n_index_alarm].week[5], &s_alarm[n_index_alarm].week[6]);
                          toFlash_WriteAlarm(s_alarm[n_index_alarm], n_index_alarm);
                          san["turn"]    = s_alarm[n_index_alarm].turn;
                          san["temp"]    = s_alarm[n_index_alarm].temp;
                          san["mode"]    = s_alarm[n_index_alarm].mode;
                          san["week"]    = n_week;
                          san["time"]    = n_time;
                          san["comment"] = n_comment;
                          serializeJson(san, client);
                          client.stop();
                        }
                        else
                        {
                          san["error"] = "index tolgan";
                          serializeJson(san, client);
                          client.stop();
                        }
                      }
                      else
                      {
                        san["error"] = "malumotlar yetarli emas";
                        serializeJson(san, client);
                        client.stop();
                      }
                    }
                    //***********************
                    else if (strcmp(u_cmd, "editalarm") == 0)
                    {
                      if (doc.containsKey("index"))
                      {
                        bool n_avadata = false;
                        unsigned int alarm_index = doc["index"];
                        if (alarm_index <= (last_index_alarm-1))
                        {
                          if (doc.containsKey("comment"))
                          {
                            const char* n_comment = doc["comment"];
                            toTime_SetComment(n_comment, alarm_index);
                            san["comment"] = n_comment;
                            n_avadata = true;
                          }
                          if (doc.containsKey("time"))
                          {
                            time_t n_time = doc["time"];
                            san["time"] = n_time;
                            s_alarm[alarm_index].time = n_time;
                            n_avadata = true;
                          }
                          if (doc.containsKey("week"))
                          {
                            const char* n_week = doc["week"];
                            san["week"] = n_week;
                            sscanf(n_week, "*Y%1d,D%1d,S%1d,C%1d,P%1d,J%1d,W%1d#", &s_alarm[alarm_index].week[0], &s_alarm[alarm_index].week[1], &s_alarm[alarm_index].week[2], &s_alarm[alarm_index].week[3], &s_alarm[alarm_index].week[4], &s_alarm[alarm_index].week[5], &s_alarm[alarm_index].week[6]);
                            n_avadata = true;
                          }
                          if (doc.containsKey("temp"))
                          {
                            s_alarm[alarm_index].temp = doc["temp"];
                            san["temp"] = s_alarm[alarm_index].temp;
                            n_avadata = true;
                          }
                          if (doc.containsKey("turn"))
                          {
                            s_alarm[alarm_index].turn = doc["turn"];
                            san["turn"] = s_alarm[alarm_index].turn;
                            n_avadata = true;
                          }
                          if (doc.containsKey("mode"))
                          {
                            s_alarm[alarm_index].mode = doc["mode"];
                            san["mode"] = s_alarm[alarm_index].mode;
                            n_avadata = true;
                          }
                          if (n_avadata)
                          {
                            toFlash_WriteAlarm(s_alarm[alarm_index], alarm_index);
                            serializeJson(san, client);
                            client.stop();
                          }
                          else
                          {
                            san["error"] = "xechqanday malumot topilmadi";
                            serializeJson(san, client);
                            client.stop();
                          }
                        }
                        else
                        {
                          san["error"] = "index mavjut emas";
                          serializeJson(san, client);
                          client.stop();
                        }
                      }
                      else
                      {
                        san["error"] = "index berilmagan";
                        serializeJson(san, client);
                        client.stop();
                      }
                    }
                    //***********************
                    else if (strcmp(u_cmd, "deletalarm") == 0)
                    {
                      if (doc.containsKey("index"))
                      {
                        unsigned int n_index = doc["index"];
                        if (n_index <= (last_index_alarm-1))
                        {
                          for (int i_index = n_index; i_index < (last_index_alarm-1); i_index++)
                          {
                            toTime_SetComment(s_alarm[i_index + 1].comment, i_index);
                            s_alarm[i_index] = s_alarm[i_index + 1];
                            toFlash_WriteAlarm(s_alarm[i_index], i_index);
                          }
                          last_index_alarm--;
                          EEPROM.write(ADDRESS_LAST_INDEX_ALARM, last_index_alarm); EEPROM.commit();
                          san["index"] = n_index;
                          serializeJson(san, client);
                          client.stop();
                        }
                        else
                        {
                          san["error"] = "index notogri";
                          serializeJson(san, client);
                          client.stop();
                        }
                      }
                      else
                      {
                        san["error"] = "index topilmadi";
                        serializeJson(san, client);
                        client.stop();
                      }
                    }
                    //***********************
                    else if (strcmp(u_cmd, "getalarm") == 0)
                    {
                      if (doc.containsKey("index"))
                      {
                        unsigned int n_index = doc["index"];
                        if (n_index <= (last_index_alarm-1))
                        {
                          char    n_week[24] = {0};
                          sprintf(n_week, "*Y%d,D%d,S%d,C%d,P%d,J%d,W%d#", s_alarm[n_index].week[0], s_alarm[n_index].week[1], s_alarm[n_index].week[2], s_alarm[n_index].week[3], s_alarm[n_index].week[4], s_alarm[n_index].week[5], s_alarm[n_index].week[6]);
                          san["comment"] = s_alarm[n_index].comment;
                          san["time"]    = s_alarm[n_index].time;
                          san["turn"]    = s_alarm[n_index].turn;
                          san["temp"]    = s_alarm[n_index].temp;
                          san["mode"]    = s_alarm[n_index].mode;
                          san["week"]    = n_week;
                          serializeJson(san, client);
                          client.stop();
                        }
                        else
                        {
                          san["error"] = "index topilmadi";
                          serializeJson(san, client);
                          client.stop();
                        }
                      }
                      else
                      {
                        san["error"] = "index topilmadi";
                        serializeJson(san, client);
                        client.stop();
                      }
                    }
                    //***********************
                    else if (strcmp(u_cmd, "getalarmall") == 0)
                    {
                      san["available"] = last_index_alarm;
                      for (int i_index = 0; i_index <= (last_index_alarm-1); i_index++)
                      {
                        char    u_week[24] = {0};
                        sprintf(u_week, "*Y%d,D%d,S%d,C%d,P%d,J%d,W%d#", s_alarm[i_index].week[0], s_alarm[i_index].week[1], s_alarm[i_index].week[2], s_alarm[i_index].week[3], s_alarm[i_index].week[4], s_alarm[i_index].week[5], s_alarm[i_index].week[6]);
                        san["alarm"][i_index]["comment"] = s_alarm[i_index].comment;
                        san["alarm"][i_index]["time"]    = s_alarm[i_index].time;
                        san["alarm"][i_index]["turn"]    = s_alarm[i_index].turn;
                        san["alarm"][i_index]["temp"]    = s_alarm[i_index].temp;
                        san["alarm"][i_index]["mode"]    = s_alarm[i_index].mode;
                        san["alarm"][i_index]["week"]    = u_week;
                        san["alarm"][i_index]["index"]   = i_index;
                      }
                      serializeJson(san, client);
                      client.stop();
                    }
                    //***********************
                    else if (strcmp(u_cmd, "getmodeall") == 0)
                    {
                      //san["available"] = last_index_alarm;
                      int n_send_index = 0;
                      //san["isactive"] = isActive;
                      for (int i_index = 0; i_index <= (last_index_alarm-1); i_index++)
                      {
                        if (s_alarm[i_index].turn == true)
                        {
                          char    u_week[24] = {0};
                          sprintf(u_week, "*Y%d,D%d,S%d,C%d,P%d,J%d,W%d#", s_alarm[i_index].week[0], s_alarm[i_index].week[1], s_alarm[i_index].week[2], s_alarm[i_index].week[3], s_alarm[i_index].week[4], s_alarm[i_index].week[5], s_alarm[i_index].week[6]);
                          san["alarm"][n_send_index]["time"]    = s_alarm[i_index].time;
                          san["alarm"][n_send_index]["mode"]    = s_alarm[i_index].mode;
                          san["alarm"][n_send_index]["week"]    = u_week;
                          if ((isActive != 0) && (isActive-1 == i_index))
                          {
                            san["alarm"][n_send_index]["isactive"] = true;
                          }
                          else
                          {
                            san["alarm"][n_send_index]["isactive"] = false;
                          }
                          // if (s_alarm[i_index].week[rtc.getDayofWeek()] == 1)
                          // {
                          //   san["alarm"][n_send_index]["istoday"] = true;
                          // }
                          // else
                          // {
                          //   san["alarm"][n_send_index]["istoday"] = false;
                          // }
                          n_send_index++;
                          //san["alarm"][i_index]["index"]   = i_index;
                        }
                      }
                      serializeJson(san, client);
                      client.stop();
                    }
                    //***********************
                    else if (strcmp(u_cmd, "testtemp") == 0)
                    {
                      if (doc.containsKey("temp"))
                      {
                        vibration_test = doc["temp"];
                        san["temp"]   = vibration_test;
                        serializeJson(san, client);
                        client.stop();
                      }
                      else
                      {
                        san["error"] = "temp topilmadi";
                        serializeJson(san, client);
                        client.stop();
                      }
                    }
                    //***********************
                    else if (strcmp(u_cmd, "stop") == 0)
                    {
                      vibration_stop   = 1;
                      san["ok"] = "stop";
                      serializeJson(san, client);
                      client.stop();
                    }
                    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                    else
                    {
                        san["error"] = "buyruq notogri";
                        serializeJson(san, client);
                        client.stop();
                    }
                    //************************************************************************************************
                  }
                  else
                  {
                    san["error"] = "login yoki parol notogri";
                    serializeJson(san, client);
                    client.stop();
                  }
                }
                else
                {
                  san["error"] = "login yoki parol topilmadi";
                  serializeJson(san, client);
                  client.stop();
                }
              }
            }
            else
            {
              san["error"] = "buyruq topilmadi";
              serializeJson(san, client);
              client.stop();
            }
          }
        }
        if (millis() - time_out > 10000)
        {
          client.stop();
        }
        vTaskDelay(1);
      }
      Serial.println("accepted: ");
      serializeJson(doc, Serial);
      Serial.println();
      Serial.println("response: ");
      serializeJson(san, Serial);
      Serial.println();
    }
    if ((WiFi.status() != WL_CONNECTED) && (connect_mode == WIFI_STA))
    {
      toConnect_Connect();
    }
    if (connect_mode == WIFI_AP)
    {
      if (last_ap_sta_num != WiFi.softAPgetStationNum())
      {
        last_ap_sta_num = WiFi.softAPgetStationNum();
        wifi_sta_list_t stationList;
        esp_wifi_ap_get_sta_list(&stationList);
        Serial.print("n of connected stations: ");
        Serial.println(stationList.num);
        for(int i = 0; i < stationList.num; i++)
        {
          wifi_sta_info_t station = stationList.sta[i];
          for(int j = 0; j< 6; j++)
          {
            char str[3];
            sprintf(str, "%02x", (int)station.mac[j]);
            Serial.print(str);
            if(j<5)
            {
              Serial.print(":");
            }
          }
          Serial.println();
        }
        Serial.println("-----------------");
      }
    }
    vTaskDelay(20);
  }
}