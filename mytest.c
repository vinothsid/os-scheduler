#include "mythread.h"
#include <signal.h>

mythread_t tcb1;
int tid_global;
void *yieldThread1() {
	int count = 100;
	while(count--) {	
		write(1,"thread1\n",strlen("thread1\n"));
		usleep(10000);
	}
	//printf("thread1 id: %d\n",(pid_t) syscall(SYS_gettid) );
	
/*	while(*(mythread_readyq())==NULL || (*mythread_readyq())->next == NULL ) {
		write(1,"1 spinning\n",strlen("1 spinning\n")); 
		usleep(10000);
	}
*/
//	tid_global = mythread_self()->tid;
//	mythread_enter_kernel();
//	mythread_t *readyQ = mythread_readyq();
//	mythread_block(readyQ,BLOCKED);
//	tcb1 = mythread_self();
//	sleep(1);
//	futex_down(&(mythread_self()->block));
//	usleep(209000);
	write(1,"thread11\n",strlen("thread11\n"));
}

int findNumThr(mythread_t headp) {
	mythread_queue_t tmp = *(mythread_runq());
	int count=0;
	while(tmp!=NULL) {
		count++;
		tmp = tmp->next;
	}
	return count;
}

void *yieldThread2() {

        int count = 100;
        while(count--) {
                write(1,"thread2\n",strlen("thread1\n"));
                usleep(1000);
        }

/*	write(1,"thread2\n",strlen("thread1\n"));
	printf("thread2 id: %d\n",(pid_t) syscall(SYS_gettid) );

	while(tid_global == 0);*/
//	syscall(SYS_tgkill,getpid(),tid_global,SIGUSR1);
//	syscall(SYS_tgkill,-1,(pid_t) syscall(SYS_gettid),SIGUSR1);

//	mythread_t runq = *(mythread_runq());
//	while(findNumThr(runq) <2) ;
//	mythread_enter_kernel();
//	mythread_deq(mythread_runq(),tcb1);
//	mythread_enq(mythread_readyq(),tcb1);
//	mythread_leave_kernel();
/*	while(*(mythread_readyq())==NULL || (*mythread_readyq())->next == NULL ) {
		write(1,"2 spinning\n",strlen("1 spinning\n")); 
		mythread_t runq = *(mythread_runq());
		printf("Num threads in runQ is : %d\n",findNumThr(runq));
		mythread_t readyq = *(mythread_readyq());
		printf("Num threads in readyQ is : %d\n",findNumThr(readyq));
		usleep(10000);
	}
*/
//	usleep(209000);
	write(1,"thread22\n",strlen("thread12\n"));
}

void *yieldThread3() {

        int count = 100;
        while(count--) {
                write(1,"thread3\n",strlen("thread1\n"));
                usleep(1000);
        }
/*
	write(1,"thread3\n",strlen("thread1\n"));
	printf("thread3 id: %d\n",(pid_t) syscall(SYS_gettid) );
*/	
//	usleep(209000);
	write(1,"thread33\n",strlen("thread12\n"));
}
void *yieldThread4() {

        int count = 100;
        while(count--) {
                write(1,"thread4\n",strlen("thread1\n"));
                usleep(1000);
        }

/*
	write(1,"thread4\n",strlen("thread1\n"));
	printf("thread4 id: %d\n",(pid_t) syscall(SYS_gettid) );

	usleep(209000);
*/
	write(1,"thread44\n",strlen("thread12\n"));

}

int main() {
	mythread_setconcurrency(1);
	//mythread_t fake=malloc(sizeof(struct mythread));
	//memset(fake,0,sizeof(struct mythread));
	//mythread_q_init(mythread_readyq(),fake);
        mythread_t tid1,tid2,tid3,tid4;
        tid1 = malloc(sizeof(struct mythread));
        tid2 = malloc(sizeof(struct mythread));
        tid3 = malloc(sizeof(struct mythread));
        tid4 = malloc(sizeof(struct mythread));

	mythread_attr_t attr1;
	mythread_attr_init(&attr1);
	attr1.attr=6;
	struct sched_param param1;
	param1.__sched_priority = 6;
	//mythread_attr_setschedparam(&attr1,&param1);

	mythread_attr_t attr2;
	mythread_attr_init(&attr2);
	attr2.attr=3;
	struct sched_param param2;
	param2.__sched_priority = 3;
	//mythread_attr_setschedparam(&attr2,&param2);


        mythread_create(&tid1,&attr1,yieldThread1,NULL);
	usleep(1000);


	mythread_create(&tid2,&attr2,yieldThread2,NULL);
	usleep(100);
	//getMember(*(mythread_runq(),block)
        mythread_create(&tid3,NULL,yieldThread3,NULL);
	usleep(100);
        mythread_create(&tid4,NULL,yieldThread4,NULL);
/*
        mythread_mutex_destroy(&lock);
        mythread_mutex_init(&lock,NULL);
        mythread_create(&tid1,NULL,yieldThread1,NULL);
        mythread_create(&tid2,NULL,yieldThread2,NULL);
        mythread_join(tid1,NULL);
        mythread_join(tid2,NULL);
*/
//	sleep(1);
//	usleep(1000);
//	printf("unblocking a thread\n");
//	mythread_enter_kernel();
//	mythread_unblock(mythread_readyq(),2);
	//mythread_unblock(mythread_readyq(),2);
        mythread_exit(NULL);

}
