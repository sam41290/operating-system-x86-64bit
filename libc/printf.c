



#include <stdarg.h>
#include <sys/defs.h>
#include<sys/kprintf.h>

char args[1024];



void printfflushchar(const char ch)
{
	
	uint64_t arg=(uint64_t)ch;
	
	unsigned long syscallnumber=99;
	
	//kprintf("%d",syscallnumber);
	
	
	__asm__(
	"movq %0,%%rax;\n"
	"movq %1,%%rbx;\n"
	"int $0x80;\n"
	:
	:"g"(syscallnumber),"g"(arg)
	);
//	kprintf("2");
	return;
	

}

void printfflushNewLine(){
	//checkForScroll();
	
	char ch='\n';
	
	unsigned long syscallnumber=99;
	
	unsigned long arg=(uint64_t)ch;
	__asm__(
	"movq %0,%%rax;\n"
	"movq %1,%%rbx;\n"
	"int $0x80;\n"
	:
	:"m"(syscallnumber),"m"(arg)
	);
	return;
	
}

int printfflush()
{
	uint64_t arg=(uint64_t)(&args);
	
	unsigned long syscallnumber=99;
	
	//kprintf("%d",syscallnumber);
	
	
	__asm__(
	"movq %0,%%rax;\n"
	"movq %1,%%rbx;\n"
	"int $0x80;\n"
	:
	:"g"(syscallnumber),"g"(arg)
	);
	uint64_t ret=0;
	
	__asm__(
	"movq %%rax,%0;\n"
	:"=g"(ret)
	);
	
	return ret;
}



int printfflushint(int text,int ctr)
{
	char str[1024]; int i = 0;

	if (text == 0)
	{
		str[0] = '0';
		str[1] = '\0';
		//printfflush(str);
	}

	while(text){
		str[i++] = (text%10) + '0';
		text/=10;
	}
	str[i] = '\0';
	//kprintf("\n%d %s",text,str);
	//reverse
	int j = 0; i--;
	while(j < i)
	{
		char temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		j++;i--;
	}
	//kprintf("\n%s",str);
	//str[j]='\0';
	//kprintf("\n%s",str);
	for(i=0;str[i]!='\0';i++)
	{
		args[ctr]=str[i];
		ctr++;
	}
	//kprintf("%s",args);
	//printfflush(str);
	return ctr;
	
}

int printfflushhex(uint64_t num,int ctr)
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
	//ret[i]='\0';

	for(i=0;ret[i]!='\0';i++)
	{
		args[ctr]=ret[i];
		ctr++;
	}
	
	//printfflush("0x");
	//printfflush(ret);
return ctr;
}



uint64_t printf(const char *pfmt, ...)
{
	va_list pap;
    int d;
    char *s, c;
	uint64_t address;
	//kprintf("from printf: %s\n",pfmt);

	int i=0;	
   va_start(pap, pfmt);
    while (*pfmt){
		//kprintf("%s\n",pfmt);
		
	
    	char ch = (char)*pfmt;
    	char nextch = (*(pfmt+1))?(char)*(pfmt+1):'\0';
    	/*if (ch == '\n')
    	{
    		printfflushNewLine();
    	}
    	else*/ 
		if (ch == '%' && nextch == 's')
    	{
            s = va_arg(pap, char *);
			for(int j=0;s[j]!='\0';j++)
			{
				args[i]=s[j];
				i++;
			}
            //printfflush(s);
            pfmt++;
    	}
    	else if (ch == '%' && nextch == 'c')
    	{
            c = (char) va_arg(pap, int);
			args[i]=c;
			i++;
            //printfflushchar(c);
            pfmt++;
      	}
    	else if (ch == '%' && nextch == 'd')
    	{
            d = va_arg(pap, int);
			//kprintf("%d",d);
			//kprintf("\n%d",123);
            i=printfflushint(d,i);
			
            pfmt++;
    	}
    	else if (ch == '%' && nextch == 'x')
    	{
            d = va_arg(pap, int);
            i=printfflushhex(d,i);
            pfmt++;
    	}
    	else if (ch == '%' && nextch == 'p')
    	{
            address = va_arg(pap, uint64_t);
            i=printfflushhex(address,i);
            pfmt++;
    	}
    	else
    	{
			args[i]=ch;
			i++;
    		//printfflushchar(ch);
			//kprintf("%s\n",pfmt);
			//kprintf("3");
    	}
 
       	pfmt++;
    }
    va_end(pap);
	args[i]='\0';
	//printf("here\n");
	//while(1);
	int ret=printfflush();
	return ret;
	
	//kprintf("4");
}
