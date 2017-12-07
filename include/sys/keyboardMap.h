#ifndef __KEYBOARD_MAP__
#define __KEYBOARD_MAP__

char getchar();
int getCtrlKeyStatus();
void setCtrlKeyStatus(int _s);
char getKeyBoardKey(int key);
int CheckIfSpecialKey(int key);
char getScancode();

#endif