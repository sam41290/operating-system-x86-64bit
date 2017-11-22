#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int strcmp(const char *str1,const char *str2)
{

int i=0;

for(i=0;str1[i]!='\0' && str2[i]!='\0';i++)
{

if(str1[i] ==str2[i])
continue;
else
if(str1[i] <str2[i])
return -1;
else if (str1[i] >str2[i])
return 1;


}

if((str1[i] =='\0') && (str2[i]=='\0'))
return 0;
else if ((str1[i] =='\0') && (str2[i]!='\0'))
return -1;
else if ((str1[i] !='\0') && (str2[i]=='\0'))
return 1;

return 2;

}
