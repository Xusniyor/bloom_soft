#include <totime.h>
#include <Arduino.h>
#include <EEPROM.h>

alarm_def s_alarm[MAX_ALARM_INDEX]; // max uygotgich soni 10 ta
unsigned int last_index_alarm;

extern EEPROMClass f_comment_0;
extern EEPROMClass f_comment_1;
extern EEPROMClass f_comment_2;
extern EEPROMClass f_comment_3;
extern EEPROMClass f_comment_4;
extern EEPROMClass f_comment_5;
extern EEPROMClass f_comment_6;
extern EEPROMClass f_comment_7;
extern EEPROMClass f_comment_8;
extern EEPROMClass f_comment_9;

unsigned int toTime_SecToMin(unsigned long a_epoch)
{
    unsigned int u_min  = (a_epoch / 60)   % 60;
    unsigned int u_hour = (a_epoch / 3600) % 60;
    return (u_hour * 60) + u_min;
}

void toTime_SetComment(const char * a_comment, int a_index)
{
    memset(s_alarm[a_index].comment, '0', sizeof(s_alarm[a_index].comment));
    strcpy(s_alarm[a_index].comment, a_comment);
    switch (a_index)
    {
    case 0:
        memset(s_alarm[0].comment, '\0', sizeof(s_alarm[0].comment));
        strcpy(s_alarm[0].comment, a_comment);
        f_comment_0.writeString(0, s_alarm[0].comment); f_comment_0.commit();
        break;
    case 1:
        memset(s_alarm[1].comment, '\0', sizeof(s_alarm[1].comment));
        strcpy(s_alarm[1].comment, a_comment);
        f_comment_1.writeString(0, s_alarm[1].comment); f_comment_1.commit();
        break;
    case 2:
        memset(s_alarm[2].comment, '\0', sizeof(s_alarm[2].comment));
        strcpy(s_alarm[2].comment, a_comment);
        f_comment_2.writeString(0, s_alarm[2].comment); f_comment_2.commit();
        break;
    case 3:
        memset(s_alarm[3].comment, '\0', sizeof(s_alarm[3].comment));
        strcpy(s_alarm[3].comment, a_comment);
        f_comment_3.writeString(0, s_alarm[3].comment); f_comment_3.commit();
        break;
    case 4:
        memset(s_alarm[4].comment, '\0', sizeof(s_alarm[4].comment));
        strcpy(s_alarm[4].comment, a_comment);
        f_comment_4.writeString(0, s_alarm[4].comment); f_comment_4.commit();
        break;
    case 5:
        memset(s_alarm[5].comment, '\0', sizeof(s_alarm[5].comment));
        strcpy(s_alarm[5].comment, a_comment);
        f_comment_5.writeString(0, s_alarm[5].comment); f_comment_5.commit();
        break;
    case 6:
        memset(s_alarm[6].comment, '\0', sizeof(s_alarm[6].comment));
        strcpy(s_alarm[6].comment, a_comment);
        f_comment_6.writeString(0, s_alarm[6].comment); f_comment_6.commit();
        break;
    case 7:
        memset(s_alarm[7].comment, '\0', sizeof(s_alarm[7].comment));
        strcpy(s_alarm[7].comment, a_comment);
        f_comment_7.writeString(0, s_alarm[7].comment); f_comment_7.commit();
        break;
    case 8:
        memset(s_alarm[8].comment, '\0', sizeof(s_alarm[8].comment));
        strcpy(s_alarm[8].comment, a_comment);
        f_comment_8.writeString(0, s_alarm[8].comment); f_comment_8.commit();
        break;
    case 9:
        memset(s_alarm[9].comment, '\0', sizeof(s_alarm[9].comment));
        strcpy(s_alarm[9].comment, a_comment);
        f_comment_9.writeString(0, s_alarm[9].comment); f_comment_9.commit();
        break;
    default:
        break;
    }
}
