#include <stdio.h>
#include <sys/kprintf.h>
#include <sys/task_manager.h>
void test()
{
// int a=123;
//kprintf("b");
//use puts
// a=printf("hello world!!!%d\n",a);
// kprintf("%d",a);
while(1);
return;
}
/*

void context1()
{
	int i=10;
	kprintf("in context 1.0 %d\n",i);
	//while(1);
	kernel_switch_to();
	kprintf("in context 1.1 %d\n",i);
	kernel_switch_to();
	kprintf("in context 1.2 %d\n",i);
	kernel_switch_to();
	kprintf("in context 1.3 %d\n",i);
	while(1);
	return;
}

void context2()
{
	int j=11;
	kprintf("in context 2.0 %d\n",j);
	//while(1);
	kernel_switch_to();
	kprintf("in context 2.1 %d\n",j);
	//while(1);
	kernel_switch_to();
	kprintf("in context 2.2 %d\n",j);
	while(1);
	//kernel_switch_to();
	return;
}*/