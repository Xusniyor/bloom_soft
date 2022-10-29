#ifndef __TOTIME_H_
#define __TOTIME_H_

#define MAX_ALARM_INDEX  10


typedef struct
{
  bool         turn;                        // faoligi
  bool         lock;                        // qulusf
  bool         mode;                        // uygotish rejimi
  int          week[7];                     // YAKSHANBA, DUSHANBA, SESHANBA, CHORSHANBA, PAYSHANBA, JUMA, SHANBA
  unsigned int temp;                        // vibiratsiya
  unsigned int time;                        // vaqt
  char         comment[64];                 // izhoz
} alarm_def;

unsigned int toTime_SecToMin(unsigned long a_epoch);
void toTime_SetComment(const char * a_comment, int a_index);

#endif /*__TOTIME_H_*/