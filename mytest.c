#include "mythread.h"

void *yieldThread1() {
	write(1,"thread1\n",strlen("thread1\n"));
	printf("id : : %d\n",mythread_self()->tid);
	usleep(209000);
	write(1,"thread11\n",strlen("thread11\n"));
}

void *yieldThread2() {
	write(1,"thread2\n",strlen("thread1\n"));
	usleep(209000);
	write(1,"thread22\n",strlen("thread12\n"));
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
        mythread_create(&tid1,NULL,yieldThread1,NULL);
	mythread_create(&tid2,NULL,yieldThread2,NULL);
        //mythread_create(&tid3,NULL,yieldThread1,NULL);
        //mythread_create(&tid4,NULL,yieldThread2,NULL);
/*
        mythread_mutex_destroy(&lock);
        mythread_mutex_init(&lock,NULL);
        mythread_create(&tid1,NULL,yieldThread1,NULL);
        mythread_create(&tid2,NULL,yieldThread2,NULL);
        mythread_join(tid1,NULL);
        mythread_join(tid2,NULL);
*/
	sleep(1);
//	usleep(1000);
//	printf("unblocking a thread\n");
//	mythread_enter_kernel();
//	mythread_unblock(mythread_readyq(),2);
	//mythread_unblock(mythread_readyq(),2);
        mythread_exit(NULL);

}
