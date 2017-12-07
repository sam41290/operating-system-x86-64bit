#include <sys/kprintf.h>
#include <stdarg.h>
#include <sys/defs.h>

char *video = (char*)0xb8000;

char *videostart=(char*)0xb8000;

static long data_written = 0;

void resetdatawritten(){
	data_written = 0;
}

char *getcurrdisp()
{
kprintf("returning video: %p",video);
return video;
}

void setcurrdisp(char *disp)
{
video=disp;
}

void flushLastKeyPress(char a, char b)
{
	char* timeArea = (char*)((uint64_t)videostart +160*24 + 128);
	int colour = 7;

   	char secStr[13] = "KEY PRESSED: "; int ss = 0;
   	while(ss < 13){
   		*timeArea++ = secStr[ss++];
   		*timeArea++ = colour;
   	}	
	
	*timeArea++ = a;
	*timeArea++ = colour;
	*timeArea++ = b;
	*timeArea++ = colour;
}

void flushtime(int seconds)
{
	char* timeArea = (char*)((uint64_t)videostart+160*24 + 88);
	char str[1024]; int i = 0;
	while(seconds){
		str[i++] = (seconds%10) + '0';
		seconds/=10;
	}
	str[i] = '\0';
	
	//reverse
	int j = 0; i--;
	while(j < i)
	{
		char temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		j++;i--;
	}

   	int colour = 7; int k = 0;
   	char secStr[11] = "BOOT TIME: "; int ss = 0;
   	while(ss < 11){
   		*timeArea++ = secStr[ss++];
   		*timeArea++ = colour;
   	}
    while( str[k] != '\0' && str[k] != '\n')
    {
        *timeArea++ = str[k++];
        *timeArea++ = colour;
    }	
}

void checkForScroll(){
	if (data_written >= 80*24)		//Last line for clock and keypress
	{
		int shift = 160*12;
		for(char* temp = (char*)videostart; temp < (char*)videostart+160*12;){
			*temp = *(temp+shift);
			temp++;
			// *temp++ = 7;
			*temp = *(temp+shift);
			temp++;
		}		

		//Clear Screen
		for(char* temp = (char*)videostart+160*12; temp < (char*)videostart+160*24;){
			*temp++ = ' ';
			*temp++ = 7;
		}		

		video = (char*)videostart+160*12;
		data_written = 80*12;
	}

	return;

	if (data_written >= 80*24)		//Last line for clock and keypress
	{
		//Scroll
		char* oldLocation = (char*)(video + 0xa0); char* temp2;
		for(temp2 = (char*)video; temp2 < (char*)video+160*24; temp2 += 2){
			*temp2 = *oldLocation;
			*(temp2+1) = 7;
			oldLocation+=2;
		}

		video = temp2;
		data_written -= 80;
		
		temp2++;
		while(temp2 < (char*)video+160*24){
			*temp2 = 7;
			temp2+=2;
		}
	}
}

void flushbackspace(){

	if (data_written == 0)
	{
		return;
	}
	video = video-2;
    *video++ = ' ';
    *video++ = 7;
    video = video-2;
    data_written--;

}



void flush(const char* text)
{
	checkForScroll();
   	int colour = 7;
    while( *text != 0 && *text != '\n')
    {
        *video++ = *text++;
        *video++ = colour;
        data_written++;
    }	
    if (*text == '\n')
    {
 		while(data_written%80 != 0){
			video+=2;
			data_written++;
		}   	
    }
}

void flushchar(const char ch)
{
	checkForScroll();

    *video++ = ch;
    *video++ = 7;
    data_written++;
}

void flushcolor(const char* text)
{
	checkForScroll();
   	int colour = 3;
    while( *text != 0 && *text != '\n')
    {
        *video++ = *text++;
        *video++ = colour;
        data_written++;
    }	
    if (*text == '\n')
    {
 		while(data_written%80 != 0){
			video+=2;
			data_written++;
		}   	
    }
}

void flushint(int text)
{
	char str[1024]; int i = 0;

	if (text == 0)
	{
		str[0] = '0';
		str[1] = '\0';
		return flush(str);
	}

	while(text){
		str[i++] = (text%10) + '0';
		text/=10;
	}
	str[i] = '\0';
	
	//reverse
	int j = 0; i--;
	while(j < i)
	{
		char temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		j++;i--;
	}

	flush(str);
}

void flushhex(uint64_t num)
{
	char ret[1024];
	int r = 0;

	do
	{
		int rem = num%16;
		if (rem >= 10)
		{
			ret[r++] = (char)('a' + rem-10);
		}
		else
		{
			ret[r++] = rem + '0';
		}
		num/=16;
	}while(num);
	ret[r] = '\0';

	//reverse
	r--;
	int i = 0;
	while(i < r)
	{
		char temp = ret[i];
		ret[i] = ret[r];
		ret[r] = temp;
		i++;r--;		
	}

	flush("0x");
	flush(ret);

}

void flushNewLine(){
	checkForScroll();
	while(data_written%80 != 0){
		*(video+1) = 7;
		video+=2;
		data_written++;
	}
}

void kprintf(const char *fmt, ...)
{
	va_list ap;
    int d;
    char *s, c;
	uint64_t address;

   va_start(ap, fmt);
    while (*fmt){
    	char ch = (char)*fmt;
    	char nextch = (*(fmt+1))?(char)*(fmt+1):'\0';
    	if (ch == '\n')
    	{
    		flushNewLine();
    	}
    	else if (ch == '%' && nextch == 's')
    	{
            s = va_arg(ap, char *);
            flush(s);
            fmt++;
    	}
    	else if (ch == '%' && nextch == 'c')
    	{
            c = (char) va_arg(ap, int);
            flushchar(c);
            fmt++;
      	}
    	else if (ch == '%' && nextch == 'd')
    	{
            d = va_arg(ap, int);
            flushint(d);
            fmt++;
    	}
    	else if (ch == '%' && nextch == 'x')
    	{
            d = va_arg(ap, int);
            flushhex(d);
            fmt++;
    	}
    	else if (ch == '%' && nextch == 'p')
    	{
            address = va_arg(ap, uint64_t);
            flushhex(address);
            fmt++;
    	}
    	else
    	{
    		flushchar(ch);
    	}
 
       	fmt++;
    }
    va_end(ap);
}

void blankpage(){

	video = videostart;
	data_written = 0;
	while(video <= videostart + 160*24){
		*video++ = ' ';
		*video++ = 7;
	}
	video = videostart;
}
