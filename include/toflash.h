#ifndef __TOFLASH_H_
#define __TOFLASH_H_

#include <totime.h>

#define MAX_LENGTH_FLASH   64

#define MAX_LENGTG_COMMENT 64

#define IS_DIFFERENCE   's'
// xotira address lari
#define EEPROM_SIZE              5
#define ADDRESS_DEFAULT          0
#define ADDRESS_CONNECT_MODE     1
#define ADDRESS_LAST_INDEX_ALARM 2
#define ADDRESS_AUTO_UPDATE      3

void toFlash_Init(void);
void toFlash_ToDefault(void);
void toFlash_ReadAll(void);

alarm_def toFlash_ReadAlarm(char a_index);
void toFlash_WriteAlarm(alarm_def a_alarm, char a_index);

#endif /*__TOFLASH_H_*/