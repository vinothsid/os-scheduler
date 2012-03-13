#include "mythread.h"
#include <signal.h>

mythread_t tid1,tid2,tid3,tid4;
mythread_attr_t *attr1,*attr2,*attr3,*attr4;
struct sched_param param1,param2,param3,param4;

void *thread1(void *changePrio ) {
	int flag = *(int *)changePrio;
	int count = 50;
        struct sched_param param1,oldprio,newprio;
        param1.__sched_priority = 99;
	while(count--) {	
		write(1,"thread1\n",strlen("thread1\n"));
		if(count == 25 && flag == 1) {
				mythread_enter_kernel();
				mythread_attr_getschedparam(attr1,&oldprio);
	        		mythread_attr_setschedparam(attr1,&param1);				 
				mythread_attr_getschedparam(attr1,&newprio);
				printf("Priority of thread1 changed from %d to : %d\n",oldprio.__sched_priority,newprio.__sched_priority);
			
//				print_queues();
				mythread_leave_kernel_nonpreemptive();

		}
		usleep(1000);
	}
	write(1,"thread1 finished\n",strlen("thread1 finished\n"));
}

void *thread2() {

        int count = 50;
        while(count--) {
                write(1,"thread2\n",strlen("thread1\n"));
                usleep(1000);
        }
	write(1,"thread2 finished\n",strlen("thread2 finished\n"));
}

void *thread3() {

        int count = 50;
        while(count--) {
                write(1,"thread3\n",strlen("thread1\n"));
                usleep(1000);
        }
	write(1,"thread3 finished\n",strlen("thread3 finished\n"));
}
void *thread4() {

        int count = 50;
        while(count--) {
                write(1,"thread4\n",strlen("thread4\n"));
                usleep(1000);
        }

	write(1,"thread4 finished\n",strlen("thread4 finished\n"));

}

void testCase(int concurrency,int prio1,int prio2,int prio3,int prio4,int changePrioDynamic ) {

	mythread_setconcurrency(concurrency);
	mythread_attr_t *pr1 = NULL;
	mythread_attr_t *pr2 = NULL;
	mythread_attr_t *pr3 = NULL;
	mythread_attr_t *pr4 = NULL;


	if (prio1 != 10 ) {
		mythread_attr_init(attr1);
		param1.__sched_priority = prio1;
		mythread_attr_setschedparam(attr1,&param1);
		pr1 = attr1;	
	}

	if(prio2 != 10) {	
		mythread_attr_init(attr2);
		param2.__sched_priority = prio2;
		mythread_attr_setschedparam(attr2,&param2);
		pr2 = attr2;	
	}
	if(prio3 != 10) {	
		mythread_attr_init(attr3);
		param3.__sched_priority = prio3;
		mythread_attr_setschedparam(attr3,&param3);
		pr3 = attr3;	
	}
	if(prio4 != 10) {	
		mythread_attr_init(attr4);
		param4.__sched_priority = prio4;
		mythread_attr_setschedparam(attr4,&param4);
		pr4 = attr4;	
	}
        mythread_create(&tid1,pr1,thread1,&changePrioDynamic);
	usleep(100);
	mythread_create(&tid2,pr2,thread2,NULL);
	usleep(100);
        mythread_create(&tid3,pr3,thread3,NULL);
	usleep(100);
        mythread_create(&tid4,pr4,thread4,NULL);

        mythread_join(tid1,NULL);
        mythread_join(tid2,NULL);
        mythread_join(tid3,NULL);
        mythread_join(tid4,NULL);

	write(1,"TEST COMPLETE\n",strlen("TEST COMPLETE\n"));
}

int main() {

//	mythread_q_init(mythread_readyq(),fake);
        tid1 = malloc(sizeof(struct mythread));
        tid2 = malloc(sizeof(struct mythread));
        tid3 = malloc(sizeof(struct mythread));
        tid4 = malloc(sizeof(struct mythread));

	//mythread_attr_t attr1;
	//
	attr1 = malloc(sizeof(mythread_attr_t));
	attr2 = malloc(sizeof(mythread_attr_t));
	attr3 = malloc(sizeof(mythread_attr_t));
	attr4 = malloc(sizeof(mythread_attr_t));


	printf("Test Case:1 , concurrency: 1, 4 Threads with equal priority\n");
	usleep(1000);
	testCase(1,10,10,10,10,0);

		
	printf("Test Case:2 , concurrency: 1, 4 Threads with unique priority. High priority thread finishes then next thread starts\n");
	usleep(1000);
	testCase(1,7,8,9,10,0);
		
	printf("Test Case:3 ,concurrency: 1, 4 Threads. 3 with equal priority and 1 thread with higher priority created later.When the high priority thread enters the system it shoud run to completion\n");
	usleep(1000);
	testCase(1,10,10,10,3,0);


	printf("Test Case:4 , concurrency: 1, 3 Threads with equal priority , thread1's priority is decreased in half-way execution. Hence thread1 should finish at the end\n");
	usleep(1000);
	testCase(1,9,9,9,9,1);

	printf("================\nRepeat the above 4 test cases with concurrency 2\n===============\n");

	printf("Test Case:5 , concurrency: 2, 4 Threads with equal priority\n");
	usleep(1000);
	testCase(2,10,10,10,10,0);
		
	printf("Test Case:6 , concurrency: 2, 4 Threads with unique priority. High priority thread finishes then next thread starts\n");
	usleep(1000);
	testCase(2,7,9,7,9,0);
		
	printf("Test Case:7 ,concurrency: 2, 4 Threads. 3 with equal priority and 1 thread with higher priority created later.When the high priority thread enters the system it shoud run to completion\n");
	usleep(1000);
	testCase(2,10,10,10,3,0);


	printf("Test Case:8 , concurrency: 2, 3 Threads with equal priority , thread1's priority is decreased in half-way execution. Hence thread1 should finish at the end\n");
	usleep(1000);
	testCase(2,9,9,9,9,1);


//	printSigUserCount();
        mythread_exit(NULL);

}
