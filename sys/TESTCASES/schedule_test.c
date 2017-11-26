#if 0

#include <sys/process.h>
#include <sys/kprintf.h>


extern task_struct* idle_process;
extern task_struct* test_process;

void test_schedule(){
	
	task_struct* new_task = create_new_task();
	// kprintf("new_task %p\n", new_task);
	test_process = new_task;

	add_to_task_list(new_task);
	// schedule_idle_task();

	// task_struct* next = get_ready_task();
	// if (next != NULL)
	// {
	// 	kprintf("Scedule succesfull for pid %d\n", next->pid);
	// 	// schedule(next);
	// }
}

#endif