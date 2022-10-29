#include <EEPROM.h>
#include <toconfig.h>
#include <toflash.h>

extern EEPROMClass  f_user_login;
extern EEPROMClass  f_user_password;

extern char user_login   [MAX_LENGTH_FLASH];
extern char user_password[MAX_LENGTH_FLASH];

void toCmd_NewUser(const char * a_login, const char * a_password)
{
    memset(user_login,     '\0', sizeof(user_login));
    memset(user_password,  '\0', sizeof(user_password));
    strcpy(user_login,      a_login);
    strcpy(user_password,   a_password);
    f_user_login.writeString   (0, user_login);    f_user_login.commit();
    f_user_password.writeString(0, user_password); f_user_password.commit();
}