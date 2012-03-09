#include "mythread.h"
#include "mysched.h"
#include <sys/time.h>
#include <signal.h>
#include <sys/syscall.h>
int sigusr1_once=0;
struct itimerval* alrm_struct;
struct sigaction alrm_handler;
struct sigaction only_block;
int count=0;
int gdbChk=5;
int INIT_SCHED=0;
void mythread_leave_kernel(void) {
	mythread_leave_kernel_nonpreemptive();

}

static void mythread_sighandler(int sig,siginfo_t *siginfo, void *ucp) {
	printf("sigalrm\n");
	if((*mythread_readyq()!=NULL)&&(mythread_t)(*mythread_readyq())->item!=NULL)
		printf("%d : %d thread id is : %u\n",sig,count,((mythread_t)((*mythread_readyq())->item))->tid);
	printf("%d : %d thread id is : %u\n",sig,count,((mythread_t)((*mythread_runq())->item))->tid);
	count++;
	//((mythread_t)((*mythread_runq()))->item)->reschedule=8;
	if(sigusr1_once) {
		printf("%d : %d killed thread id is : %ld\n",sig,count,((mythread_t)((*mythread_runq())->next->item))->tid);
		//kill(((mythread_t)((*mythread_runq())->next->item))->tid,SIGUSR1);
		syscall(SYS_tgkill,((mythread_t)(-1,(*mythread_runq())->next->item))->tid,SIGUSR1);
		sigusr1_once=0;
	}
	
	mythread_enter_kernel();
	mythread_block(mythread_readyq(),2);
	if(sig==SIGUSR1) {
		write(1,"received sigusr1\n",strlen("received sigusr1\n"));
		sleep(1);
	}
}

void mythread_init_sched(void) {
	static int count =1;
	gdbChk = count++;
	if(!INIT_SCHED) {
		printf("init sched called in %ld\n",mythread_self());	
		alrm_struct=malloc(sizeof(struct itimerval));
		(alrm_struct->it_interval).tv_sec=0;
		(alrm_struct->it_interval).tv_usec=10000;
		(alrm_struct->it_value).tv_sec=0;
		(alrm_struct->it_value).tv_usec=10000;
		setitimer(ITIMER_REAL,alrm_struct,NULL);
		write(1,"init sched\n",strlen("init sched\n"));
		memset(&alrm_handler,0,sizeof(alrm_handler));
		sigemptyset(&alrm_handler.sa_mask);
		sigaddset(&alrm_handler.sa_mask,SIGALRM);
		sigaddset(&alrm_handler.sa_mask,SIGUSR1);
		alrm_handler.sa_flags=SA_RESTART;
		alrm_handler.sa_handler=mythread_sighandler;
		sigaction(SIGALRM,&alrm_handler,NULL);
		memset(&only_block,0,sizeof(only_block));
		sigemptyset(&only_block.sa_mask);
		sigaddset(&only_block.sa_mask,SIGALRM);
		sigaddset(&only_block.sa_mask,SIGUSR1);
		only_block.sa_flags=SA_RESTART;
		only_block.sa_handler=mythread_sighandler;
		sigaction(SIGUSR1,&only_block,NULL);
		INIT_SCHED=1;
		//mythread_exit(NULL);
	}
	//mythread_enter_kernel();
	//mythread_block(mythread_readyq(),2);
	//write(1,"exit init sched\n",strlen("exit init sched\n"));
	
}

void mythread_exit_sched(void) {
	write(1,"exit sched\n",strlen("init sched\n"));
}



