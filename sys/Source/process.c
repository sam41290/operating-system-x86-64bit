//Process related 
#include <sys/process.h>
#include <sys/kmalloc.h>
#include <sys/kprintf.h>
#include <sys/paging.h>


task_struct* task_list = NULL;
task_struct* idle_process = NULL;
task_struct* test_process = NULL;
task_struct* current_scheduled = NULL;
uint64_t pid_count = 0;


void idle_loop(void){

	int i = 4;
	kprintf("Idle Loop 1.0\n");

	schedule(idle_process, test_process);
	kprintf("Idle Loop1.1\n");

	schedule2(idle_process, test_process);
	kprintf("Idle Loop1.2\n");

	schedule2(idle_process, test_process);
	kprintf("Idle Loop1.3\n");

	while(i < 5){
		schedule2(idle_process, test_process);
		kprintf("Idle Loop1.%d\n", i++);
	}
	
	while(1){}

}

void idle_loop2(void){

		int i = 4;
		kprintf("Idle Loop 2.0\n");
		schedule2(test_process, idle_process);

		kprintf("Idle Loop2.1\n");
		schedule2(test_process, idle_process);
		
		kprintf("Idle Loop2.2\n");
		schedule2(test_process, idle_process);
		
		kprintf("Idle Loop2.3\n");

		while(i < 5){
			schedule2(test_process, idle_process);
			kprintf("Idle Loop2.%d\n", i++);
		}

		while(1){}

}

void create_idle_task(){
	task_struct* new_task = kmalloc(sizeof(task_struct));

	memset((void*)new_task->kstack, 0, 4096);
	new_task->pid = pid_count++;
	new_task->state = RUNNING;
	new_task->rsp = (uint64_t)(new_task->kstack+4096-16);
	new_task->rip = (uint64_t)&idle_loop;
	
	new_task->exit_status = 0;

	idle_process = new_task;		//Save this process as idle_process. It will be scheduled whenever the runqueue is empty
	
	// add_to_task_list(new_task);	
}

task_struct* create_new_task(){

	task_struct* new_task = kmalloc(sizeof(task_struct));

	memset((void*)new_task->kstack, 0, 4096);
	new_task->pid = pid_count++;
	new_task->state = RUNNING;
	new_task->rsp = (uint64_t)(new_task->kstack+4096-16);
	new_task->rip = (uint64_t)&idle_loop2;
	
	new_task->exit_status = 0;
	// idle_process = new_task;		//Save this process as idle_process. It will be scheduled whenever the runqueue is empty
	// kprintf("idle_process %p\n", idle_process);

	return new_task;
}

void add_to_task_list(task_struct* new_task){

	if (task_list == NULL)
	{
		task_list = new_task;
	}
	else
	{
		task_struct* last = task_list;
		while(last->next != NULL){
			last = last->next;
		}
		last->next = new_task;
	}
}

task_struct* get_ready_task(){

	if (task_list == NULL)
	{
		return NULL;
	}

	task_struct* availTask = task_list;
	task_list = task_list->next;
	return availTask;
}

void yield(){
	task_struct* next = get_ready_task();
	if (next != NULL)
	{
		kprintf("Scedule succesfull for pid %d\n", next->pid);
		schedule(current_scheduled, next);
	}
	else
	{
		// kprintf("No new process to schedule\n");
		add_to_task_list(idle_process);
		yield();					//Run the idle loop
	}	
}

void schedule_idle_task(){

	current_scheduled = idle_process;

	__asm__ __volatile__ (
		"movq %0, %%rsp;"
		::"r"(idle_process->rsp)
	);

	__asm__ __volatile__(
		"pushq %0;"
		"retq;"
		::"r"(idle_process->rip)
	);	

}

void schedule(task_struct* me, task_struct* next){

	__asm__ __volatile__ (
		"push %%rax;"
    	"push %%rbx;"
    	"push %%rcx;"
    	"push %%rdx;"
    	"push %%rbp;"
    	"push %%rsi;"
    	"push %%rdi;"
    	"push %%r8;"
    	"push %%r9;"
    	"push %%r10;"
    	"push %%r11;"
    	"push %%r12;"
    	"push %%r13;"
    	"push %%r14;"
    	"push %%r15;"
		"movq %%rsp, %0;"
  		:"=r"(me->rsp)
		:
	);

	__asm__ __volatile__ (
		"movq %0, %%rsp;"
		:
		:"r"(next->rsp)
	);

	__asm__ __volatile__(
		"push %0;"
		"retq"
		 ::"r"(next->rip)
	);	

	
}

void schedule2(task_struct* me, task_struct* next){

	__asm__ __volatile__ (
		"push %%rax;"
    	"push %%rbx;"
    	"push %%rcx;"
    	"push %%rdx;"
    	"push %%rbp;"
    	"push %%rsi;"
    	"push %%rdi;"
    	"push %%r8;"
    	"push %%r9;"
    	"push %%r10;"
    	"push %%r11;"
    	"push %%r12;"
    	"push %%r13;"
    	"push %%r14;"
    	"push %%r15;"
		"movq %%rsp, %0;"
		:"=r"(me->rsp)
		:
	);


	__asm__ __volatile__ (
		"movq %0, %%rsp;"
		:
		:"r"(next->rsp)
	);


	__asm__ volatile (
    	"pop %%r15;"	
    	"pop %%r14;"	
    	"pop %%r13;"	
    	"pop %%r12;"	
    	"pop %%r11;"	
    	"pop %%r10;"
    	"pop %%r9;"
    	"pop %%r8;"	    	
    	"pop %%rdi;"
		"pop %%rsi;"
		"pop %%rbp;"
		"pop %%rdx;"
		"pop %%rcx;"
		"pop %%rbx;"
		"pop %%rax;"
		"retq"	
		::
	);		
}
