#include <sys/kprintf.h>
#include <sys/Utils.h>

int shiftKey = 0;
int ctrlKey = 0;
char ScanCodeMap[256];

enum{
	CTRL = 1,
	SHIFT = 2	
};


//Reference : http://wiki.osdev.org/PS2_Keyboard
char getScancode() {
    
    char c=0;
    if(inb(0x60)!=c) {
        c=inb(0x60);
        if(c>0){
            return c;
        }
    }

    return (char)(0x38);					//TODO : Change this
}
 

int CheckIfSpecialKey(int key)
{
	if (key == (int)(0x1D))		//CTRL 
	{
		return CTRL;
	}
	else if (key == (int)(0x2A) || key == (int)(0x36) )			//SHIFT
	{
		return SHIFT;
	}

	return 0;
}

//Reference : http://wiki.osdev.org/PS2_Keyboard : ScanCode Set 1
void MakeKeyboardMapping(){
	ScanCodeMap[(int)(0x04)] = '3';
	ScanCodeMap[(int)(0x08)] = '7';
	ScanCodeMap[(int)(0x0C)] = '-';
	ScanCodeMap[(int)(0x10)] = 'q';
	ScanCodeMap[(int)(0x14)] = 't';
	ScanCodeMap[(int)(0x18)] = 'o';
	ScanCodeMap[(int)(0x1C)] = '#';		//ENTER
	ScanCodeMap[(int)(0x20)] = 'd';
	ScanCodeMap[(int)(0x24)] = 'j';
	ScanCodeMap[(int)(0x28)] = '\'';
	ScanCodeMap[(int)(0x2C)] = 'z';
	ScanCodeMap[(int)(0x30)] = 'b';
	ScanCodeMap[(int)(0x34)] = '.';
	ScanCodeMap[(int)(0x38)] = '#';		//LALT
	ScanCodeMap[(int)(0x3C)] = '#';		//F2
	ScanCodeMap[(int)(0x40)] = '#';		//F6
	ScanCodeMap[(int)(0x44)] = '#';		//F10
	ScanCodeMap[(int)(0x48)] = '8';
	ScanCodeMap[(int)(0x4C)] = '5';
	ScanCodeMap[(int)(0x50)] = '2';
	ScanCodeMap[(int)(0x58)] = '#';		//F12
	ScanCodeMap[(int)(0x01)] = '#';		//ESCAPE
	ScanCodeMap[(int)(0x05)] = '4';		
	ScanCodeMap[(int)(0x09)] = '8';
	ScanCodeMap[(int)(0x0D)] = '=';
	ScanCodeMap[(int)(0x11)] = 'w';
	ScanCodeMap[(int)(0x15)] = 'y';
	ScanCodeMap[(int)(0x19)] = 'p';
	ScanCodeMap[(int)(0x1D)] = '#';		//LCTRL
	ScanCodeMap[(int)(0x21)] = 'f';
	ScanCodeMap[(int)(0x25)] = 'k';
	ScanCodeMap[(int)(0x29)] = '`';
	ScanCodeMap[(int)(0x2D)] = 'x';
	ScanCodeMap[(int)(0x31)] = 'n';
	ScanCodeMap[(int)(0x35)] = '/';
	ScanCodeMap[(int)(0x39)] = ' ';
	ScanCodeMap[(int)(0x3D)] = '#';		//F3
	ScanCodeMap[(int)(0x41)] = '#';		//F7
	ScanCodeMap[(int)(0x45)] = '#';		//NUMLOCK
	ScanCodeMap[(int)(0x49)] = '9';
	ScanCodeMap[(int)(0x4D)] = '6';
	ScanCodeMap[(int)(0x51)] = '3';
	ScanCodeMap[(int)(0x02)] = '1';
	ScanCodeMap[(int)(0x06)] = '5';
	ScanCodeMap[(int)(0x0A)] = '9';
	ScanCodeMap[(int)(0x0E)] = '\b';
	ScanCodeMap[(int)(0x12)] = 'e';
	ScanCodeMap[(int)(0x16)] = 'u';
	ScanCodeMap[(int)(0x1A)] = '[';
	ScanCodeMap[(int)(0x1E)] = 'a';
	ScanCodeMap[(int)(0x22)] = 'g';
	ScanCodeMap[(int)(0x26)] = 'l';
	ScanCodeMap[(int)(0x2E)] = 'c';
	ScanCodeMap[(int)(0x32)] = 'm';
	ScanCodeMap[(int)(0x3E)] = '_';		//F4
	ScanCodeMap[(int)(0x42)] = '_';		//F8
	ScanCodeMap[(int)(0x4A)] = '#';
	ScanCodeMap[(int)(0x4E)] = '+';
	ScanCodeMap[(int)(0x52)] = '0';
	ScanCodeMap[(int)(0x03)] = '2';
	ScanCodeMap[(int)(0x07)] = '6';
	ScanCodeMap[(int)(0x0B)] = '0';
	ScanCodeMap[(int)(0x0F)] = '\t';
	ScanCodeMap[(int)(0x13)] = 'r';
	ScanCodeMap[(int)(0x17)] = 'i';
	ScanCodeMap[(int)(0x1B)] = ']';
	ScanCodeMap[(int)(0x1F)] = 's';
	ScanCodeMap[(int)(0x23)] = 'h';
	ScanCodeMap[(int)(0x27)] = ';';
	ScanCodeMap[(int)(0x2B)] = '\\';
	ScanCodeMap[(int)(0x2F)] = 'v';
	ScanCodeMap[(int)(0x33)] = ',';
	ScanCodeMap[(int)(0x37)] = '*';
	ScanCodeMap[(int)(0x3B)] = '#';		//F1
	ScanCodeMap[(int)(0x3F)] = '#';		//F5
	ScanCodeMap[(int)(0x43)] = '#';		//F9
	ScanCodeMap[(int)(0x47)] = '7';
	ScanCodeMap[(int)(0x4B)] = '4';
	ScanCodeMap[(int)(0x4F)] = '1';
	ScanCodeMap[(int)(0x53)] = '.';
	ScanCodeMap[(int)(0x57)] = '#';		//F11
	ScanCodeMap[(int)(0x1D)] = '#';		//RCTRL
	ScanCodeMap[(int)(0x35)] = '/';
	ScanCodeMap[(int)(0x38)] = '#';		//RALT
	ScanCodeMap[(int)(0x1C)] = '#';		//ENTER
	ScanCodeMap[(int)(0x53)] = '#';		//DELETE
	ScanCodeMap[(int)(0x4F)] = '#';		//END
	ScanCodeMap[(int)(0x47)] = '#';		//HOME
	ScanCodeMap[(int)(0x2A)] = '#';		//LSHIFT
	ScanCodeMap[(int)(0x36)] = '#';		//RSHIFT
	ScanCodeMap[(int)(0x3A)] = '#';		//CAPSLOCK
}

char getKeyBoardKey(int key){
	if (shiftKey == 1)
	{
		shiftKey = 0;
		return (ScanCodeMap[key]-32);
	}
	return ScanCodeMap[key];
}

char getchar() {
	char key = getScancode();

	if (CheckIfSpecialKey(key) == 2)
	{
		shiftKey = 1;
	}
	else if (CheckIfSpecialKey(key) == 1)
	{
		ctrlKey = 1;
	}
	else
	{
		return getKeyBoardKey(key);
	}    

	return '#';
}


int getCtrlKeyStatus(){
	return ctrlKey;
}

void setCtrlKeyStatus(int _s){
	ctrlKey = _s;
}