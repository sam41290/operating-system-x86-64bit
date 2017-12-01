
#include <stdarg.h>
#include <sys/defs.h>
#include<stdio.h>
//#include<sys/kprintf.h>

int printfflushint(int text,int ctr,char *args)
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

int printfflushhex(uint64_t num,int ctr,char *args)
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



int printf(const char *pfmt, ...)
{
	
	//puts("in printf\n");
	char args[1024];
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
            i=printfflushint(d,i,args);
			
            pfmt++;
    	}
    	else if (ch == '%' && nextch == 'x')
    	{
            d = va_arg(pap, int);
            i=printfflushhex(d,i,args);
            pfmt++;
    	}
    	else if (ch == '%' && nextch == 'p')
    	{
            address = va_arg(pap, uint64_t);
            i=printfflushhex(address,i,args);
            pfmt++;
    	}
    	else
    	{
			//puts("printf got char\n");
			args[i]=ch;
			i++;
			
			//puts("3");
    		//printfflushchar(ch);
			//kprintf("%s\n",pfmt);
			//kprintf("3");
    	}
		//puts("here\n");
       	pfmt++;
    }
    va_end(pap);
	args[i]='\0';
	//puts("string parsing complete\n");
	int ret=puts(args);
	return ret;
	
	//kprintf("4");
}
