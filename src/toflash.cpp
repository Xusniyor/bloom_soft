#include <EEPROM.h>
#include <WiFi.h>
#include <toflash.h>
#include <toconfig.h>
#include <toconnect.h>
#include <totime.h>
#include <otadrive_esp.h>
// xotiradan malumotlarni oqish
EEPROMClass  f_ssid_ap       ("eeprom0", MAX_LENGTH_FLASH);
EEPROMClass  f_password_ap   ("eeprom1", MAX_LENGTH_FLASH);
EEPROMClass  f_ssid_sta      ("eeprom2", MAX_LENGTH_FLASH);
EEPROMClass  f_password_sta  ("eeprom3", MAX_LENGTH_FLASH);
EEPROMClass  f_device_name   ("eeprom4", MAX_LENGTH_FLASH);
EEPROMClass  f_device_version("eeprom6", MAX_LENGTH_FLASH);
EEPROMClass  f_user_login    ("eeprom7", MAX_LENGTH_FLASH);
EEPROMClass  f_user_password ("eeprom8", MAX_LENGTH_FLASH);
// uygotgich uchun xotiradan joy ajratish
EEPROMClass  f_alarm_0       ("alarm0",  MAX_LENGTH_FLASH);
EEPROMClass  f_alarm_1       ("alarm1",  MAX_LENGTH_FLASH);
EEPROMClass  f_alarm_2       ("alarm2",  MAX_LENGTH_FLASH);
EEPROMClass  f_alarm_3       ("alarm3",  MAX_LENGTH_FLASH);
EEPROMClass  f_alarm_4       ("alarm4",  MAX_LENGTH_FLASH);
EEPROMClass  f_alarm_5       ("alarm5",  MAX_LENGTH_FLASH);
EEPROMClass  f_alarm_6       ("alarm6",  MAX_LENGTH_FLASH);
EEPROMClass  f_alarm_7       ("alarm7",  MAX_LENGTH_FLASH);
EEPROMClass  f_alarm_8       ("alarm8",  MAX_LENGTH_FLASH);
EEPROMClass  f_alarm_9       ("alarm9",  MAX_LENGTH_FLASH);

EEPROMClass f_comment_0      ("comment0", MAX_LENGTG_COMMENT);
EEPROMClass f_comment_1      ("comment1", MAX_LENGTG_COMMENT);
EEPROMClass f_comment_2      ("comment2", MAX_LENGTG_COMMENT);
EEPROMClass f_comment_3      ("comment3", MAX_LENGTG_COMMENT);
EEPROMClass f_comment_4      ("comment4", MAX_LENGTG_COMMENT);
EEPROMClass f_comment_5      ("comment5", MAX_LENGTG_COMMENT);
EEPROMClass f_comment_6      ("comment6", MAX_LENGTG_COMMENT);
EEPROMClass f_comment_7      ("comment7", MAX_LENGTG_COMMENT);
EEPROMClass f_comment_8      ("comment8", MAX_LENGTG_COMMENT);
EEPROMClass f_comment_9      ("comment9", MAX_LENGTG_COMMENT);

extern wifi_def     s_wifi_ap;  // ulanish nuqtasi
extern wifi_def     s_wifi_sta; // ulanish wifi
extern networks_def s_scan[20]; // maksimal 20 ta network
extern alarm_def    s_alarm[MAX_ALARM_INDEX]; // max uygotgich soni 10 ta
extern int  connect_mode;
extern bool auto_update;
extern int  last_index_alarm;
extern char user_login   [MAX_LENGTH_FLASH];
extern char user_password[MAX_LENGTH_FLASH];
char        device_name  [MAX_LENGTH_FLASH];

extern EEPROMClass f_url_update;
extern EEPROMClass f_key_update;

void toFlash_Init(void)
{
    Serial.println        ("flash init");
    EEPROM.begin          (EEPROM_SIZE);
    f_url_update.begin    (f_url_update.length());
    f_key_update.begin    (f_key_update.length());
    f_ssid_ap.begin       (f_ssid_ap.length());
    f_password_ap.begin   (f_password_ap.length());
    f_ssid_sta.begin      (f_ssid_sta.length());
    f_password_sta.begin  (f_password_sta.length());
    f_device_name.begin   (f_device_name.length());
    f_device_version.begin(f_device_version.length());
    f_user_login.begin    (f_user_login.length());
    f_user_password.begin (f_user_password.length());
    // uygotgich xotirasini initsalizatsiya qilish
    f_alarm_0.begin       (f_alarm_0.length());
    f_alarm_1.begin       (f_alarm_1.length());
    f_alarm_2.begin       (f_alarm_2.length());
    f_alarm_3.begin       (f_alarm_3.length());
    f_alarm_4.begin       (f_alarm_4.length());
    f_alarm_5.begin       (f_alarm_5.length());
    f_alarm_6.begin       (f_alarm_6.length());
    f_alarm_7.begin       (f_alarm_7.length());
    f_alarm_8.begin       (f_alarm_8.length());
    f_alarm_9.begin       (f_alarm_9.length());
    // uygotgich commentariyalari
    f_comment_0.begin     (f_comment_0.length());
    f_comment_1.begin     (f_comment_1.length());
    f_comment_2.begin     (f_comment_2.length());
    f_comment_3.begin     (f_comment_3.length());
    f_comment_4.begin     (f_comment_4.length());
    f_comment_5.begin     (f_comment_5.length());
    f_comment_6.begin     (f_comment_6.length());
    f_comment_7.begin     (f_comment_7.length());
    f_comment_8.begin     (f_comment_8.length());
    f_comment_9.begin     (f_comment_9.length());
}
void toFlash_ToDefault(void)
{
    if (EEPROM.read(ADDRESS_DEFAULT) != IS_DIFFERENCE)
    {
        Serial.println              ("set default settings");

        char n_buff[MAX_LENGTH_FLASH] = {0};

        sprintf(n_buff, "%s-%s", DEFAULT_DEVICE_NAME, OTADRIVE.getChipId().c_str());

        f_url_update.writeString    (0, DEFAULT_UPDATE_URL);     f_url_update.commit();
        f_key_update.writeString    (0, DEFAULT_UPDATE_KEY);     f_key_update.commit();

        f_ssid_ap.writeString       (0, n_buff);                       f_ssid_ap.commit();
        f_password_ap.writeString   (0, OTADRIVE.getChipId().c_str()); f_password_ap.commit();
        f_ssid_sta.writeString      (0, DEFAULT_STA_SSID);             f_ssid_sta.commit();
        f_password_sta.writeString  (0, DEFAULT_STA_PASSWORD);         f_password_sta.commit();
        f_device_name.writeString   (0, DEFAULT_DEVICE_NAME);          f_device_name.commit();
        f_device_version.writeString(0, DEFAULT_DEVICE_VERSION);       f_device_version.commit();
        f_user_login.writeString    (0, n_buff);                       f_user_login.commit();
        f_user_password.writeString (0, OTADRIVE.getChipId().c_str()); f_user_password.commit();

        f_alarm_0.writeString       (0, "0062,0007,21600\0");          f_alarm_0.commit();
        f_alarm_1.writeString       (0, "0000,0000,00000\0");          f_alarm_1.commit();
        f_alarm_2.writeString       (0, "0000,0000,00000\0");          f_alarm_2.commit();
        f_alarm_3.writeString       (0, "0000,0000,00000\0");          f_alarm_3.commit();
        f_alarm_4.writeString       (0, "0000,0000,00000\0");          f_alarm_4.commit();
        f_alarm_5.writeString       (0, "0000,0000,00000\0");          f_alarm_5.commit();
        f_alarm_6.writeString       (0, "0000,0000,00000\0");          f_alarm_6.commit();
        f_alarm_7.writeString       (0, "0000,0000,00000\0");          f_alarm_7.commit();
        f_alarm_8.writeString       (0, "0000,0000,00000\0");          f_alarm_8.commit();
        f_alarm_9.writeString       (0, "0000,0000,00000\0");          f_alarm_9.commit();

        f_comment_0.writeString       (0, "Xayrli tong\0");      f_comment_0.commit();
        f_comment_1.writeString       (0, " \0");                f_comment_1.commit();
        f_comment_2.writeString       (0, " \0");                f_comment_2.commit();
        f_comment_3.writeString       (0, " \0");                f_comment_3.commit();
        f_comment_4.writeString       (0, " \0");                f_comment_4.commit();
        f_comment_5.writeString       (0, " \0");                f_comment_5.commit();
        f_comment_6.writeString       (0, " \0");                f_comment_6.commit();
        f_comment_7.writeString       (0, " \0");                f_comment_7.commit();
        f_comment_8.writeString       (0, " \0");                f_comment_8.commit();
        f_comment_9.writeString       (0, " \0");                f_comment_9.commit();

        EEPROM.write(ADDRESS_DEFAULT,          IS_DIFFERENCE);
        EEPROM.write(ADDRESS_CONNECT_MODE,     WIFI_STA);
        EEPROM.write(ADDRESS_LAST_INDEX_ALARM, 1);
        EEPROM.write(ADDRESS_AUTO_UPDATE,      1);
        EEPROM.commit();
    }
}
void toFlash_ReadAll(void)
{
    Serial.println("flash read");
    f_ssid_ap.get      (0, s_wifi_ap.ssid);
    f_password_ap.get  (0, s_wifi_ap.password);
    f_ssid_sta.get     (0, s_wifi_sta.ssid);
    f_password_sta.get (0, s_wifi_sta.password);
    f_device_name.get  (0, device_name);
    f_user_login.get   (0, user_login);
    f_user_password.get(0, user_password);

    connect_mode     = EEPROM.read(ADDRESS_CONNECT_MODE);
    last_index_alarm = EEPROM.read(ADDRESS_LAST_INDEX_ALARM);

    for (int i_index = 0; i_index < last_index_alarm; i_index++)
    {
        s_alarm[i_index] = toFlash_ReadAlarm(i_index);
    }

    f_comment_0.get(0, s_alarm[0].comment);
    f_comment_1.get(0, s_alarm[1].comment);
    f_comment_2.get(0, s_alarm[2].comment);
    f_comment_3.get(0, s_alarm[3].comment);
    f_comment_4.get(0, s_alarm[4].comment);
    f_comment_5.get(0, s_alarm[5].comment);
    f_comment_6.get(0, s_alarm[6].comment);
    f_comment_7.get(0, s_alarm[7].comment);
    f_comment_8.get(0, s_alarm[8].comment);
    f_comment_9.get(0, s_alarm[9].comment);

    Serial.print("ssid AP:       "); Serial.println(s_wifi_ap.ssid);
    Serial.print("password AP:   "); Serial.println(s_wifi_ap.password);
    Serial.print("ssid STA:      "); Serial.println(s_wifi_sta.ssid);
    Serial.print("password STA:  "); Serial.println(s_wifi_sta.password);
    Serial.print("device Name:   "); Serial.println(device_name);
    Serial.print("user Login:    "); Serial.println(user_login);
    Serial.print("user Password: "); Serial.println(user_password);
    Serial.print("connect Mode:  "); Serial.println((connect_mode == 1)?"WIFI_STA":"WIFI_AP");
    Serial.print("alarm count:   "); Serial.println(last_index_alarm);

    for (int i_index = 0; i_index < last_index_alarm; i_index++)
    {
        char p_buff[64] = {0};
        sprintf(p_buff, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", s_alarm[i_index].turn, s_alarm[i_index].mode, s_alarm[i_index].temp, s_alarm[i_index].week[0], s_alarm[i_index].week[1], s_alarm[i_index].week[2], s_alarm[i_index].week[3], s_alarm[i_index].week[4], s_alarm[i_index].week[5], s_alarm[i_index].week[6], s_alarm[i_index].time);
        Serial.print("alarm index    "); Serial.print((int)i_index); Serial.print(": "); Serial.println(p_buff);
        Serial.print("comment:       "); Serial.print((int)i_index); Serial.print(": "); Serial.println(s_alarm[i_index].comment);
    }
}
alarm_def toFlash_ReadAlarm(char a_index)
{
    alarm_def    n_alarm  = {0};
    char         n_buff[MAX_LENGTH_FLASH] = {0};
    unsigned int n_config = 0;
    switch (a_index)
    {
        case 0: f_alarm_0.get(0, n_buff); break;
        case 1: f_alarm_1.get(0, n_buff); break;
        case 2: f_alarm_2.get(0, n_buff); break;
        case 3: f_alarm_3.get(0, n_buff); break;
        case 4: f_alarm_4.get(0, n_buff); break;
        case 5: f_alarm_5.get(0, n_buff); break;
        case 6: f_alarm_6.get(0, n_buff); break;
        case 7: f_alarm_7.get(0, n_buff); break;
        case 8: f_alarm_8.get(0, n_buff); break;
        case 9: f_alarm_9.get(0, n_buff); break;
    }
    sscanf(n_buff, "%d,%d,%d", &n_config, &n_alarm.temp, &n_alarm.time);
    n_alarm.turn    = 0x1 & (n_config >> 8);
    n_alarm.mode    = 0x1 & (n_config >> 7);
    n_alarm.week[6] = 0x1 & (n_config >> 6);
    n_alarm.week[5] = 0x1 & (n_config >> 5);
    n_alarm.week[4] = 0x1 & (n_config >> 4);
    n_alarm.week[3] = 0x1 & (n_config >> 3);
    n_alarm.week[2] = 0x1 & (n_config >> 2);
    n_alarm.week[1] = 0x1 & (n_config >> 1);
    n_alarm.week[0] = 0x1 & (n_config >> 0);
    return n_alarm;
}
void toFlash_WriteAlarm(alarm_def a_alarm, char a_index)
{
  char n_buff[MAX_LENGTH_FLASH] = {0};
  unsigned int n_config   = 0;
  n_config |= ((a_alarm.turn    & 0x1) << 8);
  n_config |= ((a_alarm.mode    & 0x1) << 7);
  n_config |= ((a_alarm.week[6] & 0x1) << 6);
  n_config |= ((a_alarm.week[5] & 0x1) << 5);
  n_config |= ((a_alarm.week[4] & 0x1) << 4);
  n_config |= ((a_alarm.week[3] & 0x1) << 3);
  n_config |= ((a_alarm.week[2] & 0x1) << 2);
  n_config |= ((a_alarm.week[1] & 0x1) << 1);
  n_config |= ((a_alarm.week[0] & 0x1) << 0);
  sprintf(n_buff, "%d,%d,%d", n_config, a_alarm.temp, a_alarm.time);
  switch (a_index)
  {
    case 0: f_alarm_0.writeString(0, n_buff); f_alarm_0.commit(); break;
    case 1: f_alarm_1.writeString(0, n_buff); f_alarm_1.commit(); break;
    case 2: f_alarm_2.writeString(0, n_buff); f_alarm_2.commit(); break;
    case 3: f_alarm_3.writeString(0, n_buff); f_alarm_3.commit(); break;
    case 4: f_alarm_4.writeString(0, n_buff); f_alarm_4.commit(); break;
    case 5: f_alarm_5.writeString(0, n_buff); f_alarm_5.commit(); break;
    case 6: f_alarm_6.writeString(0, n_buff); f_alarm_6.commit(); break;
    case 7: f_alarm_7.writeString(0, n_buff); f_alarm_7.commit(); break;
    case 8: f_alarm_8.writeString(0, n_buff); f_alarm_8.commit(); break;
    case 9: f_alarm_9.writeString(0, n_buff); f_alarm_9.commit(); break;
  }
}
