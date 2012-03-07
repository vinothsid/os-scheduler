#include "mythread.h"

void *yieldThread1() {
	write(1,"thread1\n",strlen("thread1\n"));
}

void *yieldThread2() {
	write(1,"thread2\n",strlen("thread1\n"));
}

int main() {
	mythread_setconcurrency(2);

        mythread_t tid1,tid2;
        tid1 = malloc(sizeof(struct mythread));
        tid2 = malloc(sizeof(struct mythread));
        mythread_create(&tid1,NULL,yieldThread1,NULL);
        mythread_create(&tid2,NULL,yieldThread2,NULL);
        mythread_join(tid1,NULL);
        mythread_join(tid2,NULL);
        mythread_create(&tid1,NULL,yieldThread1,NULL);
        mythread_create(&tid2,NULL,yieldThread2,NULL);
/*
        mythread_mutex_destroy(&lock);
        mythread_mutex_init(&lock,NULL);
        mythread_create(&tid1,NULL,yieldThread1,NULL);
        mythread_create(&tid2,NULL,yieldThread2,NULL);
        mythread_join(tid1,NULL);
        mythread_join(tid2,NULL);
*/
        mythread_exit(NULL);

}
