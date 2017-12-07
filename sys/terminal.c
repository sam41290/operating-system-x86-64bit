#include <sys/terminal.h>
#include <sys/keyboardMap.h>
#include <sys/kprintf.h>

extern struct terminal terminal_for_keyboard;


__volatile__ int READMODEON = 0;

uint64_t read(struct terminal* self, uint64_t addr){

	__asm__ __volatile__("sti");

	READMODEON = 1;

	while(READMODEON == 1){
		//wait for enter key in stdin
		//READMODE is made volatile else it is a tight loops and never breaks
		//Undoing comiler optimization	
	}
	
	#ifdef TERMINAL_DEBUG
	kprintf("Going to return to app : ");
	#endif
	for (int i = 0; i < self->stdinlength; ++i)
	{
		#ifdef TERMINAL_DEBUG
		kprintf("%c", self->stdin[i]);
		#endif
		*(uint64_t*)addr++ = self->stdin[i];
	}
	int write_count = self->stdinlength;

	//Reset stdin for next read
	for (int i = 0; i < 1024; ++i)
	{
		self->stdin[i] = ' ';
	}
	self->stdinlength = 0;

	return write_count;
}

uint64_t write(struct terminal* self, uint64_t addr, uint64_t count){

	char* str = (char*)(uint64_t*)addr;
	kprintf("%s",str);
	return count;
}

void LocalEcho(char c){
	if (getCtrlKeyStatus())
	{
		setCtrlKeyStatus(0);
		flushchar(c);
	}
	else if(c == '-')			//'\b' not working for backspace. so mapping to '-'
	{
		flushbackspace();
	}
	else
	{
		flushchar(c);
	}	
}

void tunnel(struct terminal* self){

	char c = getchar();
	if (c != '#')
	{
		//Check if READMODE is on then start buffering
		if (READMODEON == 0)
		{
			return;
		}


		
		if (self->stdinlength >= 1022)
		{
			kprintf("Cant support more than 1024 characters!\n");
			self->stdin[self->stdinlength] = '\0';
			READMODEON = 0;
		}
		else if (self->stdinlength == 0 && c == '-')
		{
			return;
		}

		//Local Echo
		LocalEcho(c);

		if (c == '\n')
		{
			READMODEON = 0;
			#ifdef TERMINAL_DEBUG
			kprintf("Enter pressed READMODEON %d", READMODEON);
			#endif
			self->stdin[self->stdinlength++] = '\0';
		}
		else if(c == '-')			//'\b' not working for backspace. so mapping to '-'
		{
			self->stdin[self->stdinlength--] = ' ';			
		}	
		else
		{
			self->stdin[self->stdinlength++] = c;
		}

	}

}


void init_terminal(){
	terminal_for_keyboard.tunnel = tunnel;
	terminal_for_keyboard.read = read;
	terminal_for_keyboard.write = write;
	terminal_for_keyboard.stdinlength = 0;
}
