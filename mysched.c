#include "mythread.h"
#include "mysched.h"

int gdbChk=5;
void mythread_leave_kernel(void) {
	mythread_leave_kernel_nonpreemptive();

}

void mythread_init_sched(void) {
	static int count =1;
	gdbChk = count++;
	write(1,"init sched\n",strlen("init sched\n"));

}

void mythread_exit_sched(void) {
	write(1,"exit sched\n",strlen("init sched\n"));
}



