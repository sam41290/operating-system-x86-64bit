#include<stdio.h>
#include<sys/kprintf.h>
#include<sys/task_manager.h>
void test()
{
int a=123;
//kprintf("b");
printf("hello world!!!%d\n",a);
while(1);
return;
}

void context1()
{
	int i=10;
	printf("in context 1.0 %d\n",i);
	//while(1);
	switch_to();
	printf("in context 1.1 %d\n",i);
	switch_to();
	printf("in context 1.2 %d\n",i);
	switch_to();
	printf("in context 1.3 %d\n",i);
	while(1);
	return;
}

void context2()
{
	int j=11;
	printf("in context 2.0 %d\n",j);
	//while(1);
	switch_to();
	printf("in context 2.1 %d\n",j);
	//while(1);
	switch_to();
	printf("in context 2.2 %d\n",j);
	while(1);
	//switch_to();
	return;
}