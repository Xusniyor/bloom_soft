#ifndef __TOUPDATE_H_
#define __TOUPDATE_H_

#define MAX_URL_KEY_LEN  64

void toUpdate_Init(void);
void toUpdate_Update(unsigned int a_toalarm);
void toUpdate_NewKey(const char * keyurl);
void toUpdate_OtaServer_Init(void);
void toUpdate_OtoServer_loop(void);
void toUpdate_Oto_Init(void);
void toUpdate_Oto_loop(void);


#endif /*__TOUPDATE_H_*/