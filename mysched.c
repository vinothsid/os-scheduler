#include "mythread.h"
#include "mysched.h"
#include <sys/time.h>
#include <signal.h>
#include <sys/syscall.h>


//int sigusr1_once=0;
struct itimerval* alrm_struct;
struct sigaction alrm_handler;
struct sigaction only_block;
//int count=0;
//int gdbChk=5;
int INIT_SCHED=0;
int resched_lock=1;
int resched_flag=0;

int dummy=0;
void mythread_leave_kernel(void) {

	lll_lock(&resched_lock);
		if(resched_flag == 1) {
#ifdef VERBOSE
			print("Resched flag is set\n");
#endif
			mythread_scheduler();
			
		}
	
	mythread_leave_kernel_nonpreemptive();
	lll_unlock(&resched_lock);

}

static int mythread_scheduler(void) {
#ifdef VERBOSE 
	printf("Scheduler called in %ld\n",mythread_self()->tid);
#endif
	mythread_queue_t *readyq = mythread_readyq();
	mythread_queue_t *runq = mythread_runq();


	mythread_queue_t ptr=*runq;
	mythread_t self = mythread_self();
	int concurrent_level = mythread_getconcurrency();

	while(ptr!=NULL) {
		if(getMember(ptr,tid) != self->tid) {
#ifdef VERBOSE
printf("Sending SIGUSR to %ld\n",getMember(ptr,tid) );
#endif
			syscall(SYS_tgkill,getpid(),getMember(ptr,tid),SIGUSR1);

		}
		ptr = ptr->next;
	}

	ptr = *runq;
	mythread_t tmp;
	mythread_queue_t next;

#ifdef DEBUG
	print("Before prempting\n");
	print("RunQ\n");
	int loc=printQ(ptr);
	print("ReadyQ\n");
	printQ(*(mythread_readyq()));
#endif
	while(ptr != NULL ) {
		//dummy++;
		next = ptr->next;
		if(getMember(ptr,tid) != self->tid) {
			tmp = ptr->item;
			mythread_deq(runq,tmp);
			mythread_enq(readyq,tmp);
#ifdef VERBOSE
			printf("Thread : %ld moved to readyq\n",tmp->tid);
#endif
		}

		ptr = next;
	}


	ptr = *mythread_readyq();
	int new_scheduled_num=0;

/*	
	// if main is present in the runq then increment concurrent level
	mythread_queue_t ptr1= *(mythread_runq());
	while( ptr1!= NULL) {
		if( getMember(ptr1,tid) == 0 ) 
			concurrent_level++;
		ptr1 = ptr1->next;
	}	
*/	 
	while(ptr!= NULL && new_scheduled_num < concurrent_level-1 ) {
		tmp = mythread_deq_prio(readyq);
		mythread_enq(runq,tmp);
		tmp->state &= ~BLOCKED;
		futex_up(&(tmp->block));
		new_scheduled_num++;
		ptr = *(mythread_readyq());
#ifdef VERBOSE
			printf("Thread : %ld moved to runq and futex up done %d\n",tmp->tid,mythread_getconcurrency());
#endif
	}
	if(!mythread_inq(mythread_runq(),self) ) { // if current is not in runq schedule another thread
		if(*(mythread_readyq()) != NULL) {
				tmp = mythread_deq_prio(readyq);
				mythread_enq(runq,tmp);
				tmp->state &= ~BLOCKED;
				futex_up(&(tmp->block));
				new_scheduled_num++;
#ifdef VERBOSE
                        	printf("Thread : %ld moved to runq and futex up done\n",tmp->tid);
#endif


		}
		//print("not in queue\n");
		resched_flag = 0;
		return 0;

	}
/*
#ifdef DEBUG
	print("After rescheduling\n");
	print("RunQ\n");
	printQ(ptr);
	print("ReadyQ\n");
	printQ(*(mythread_readyq()));
#endif
*/
	resched_flag=0;
	if(*mythread_readyq()!= NULL  ) {	
		return 1;
		
	}	
	else {
		return 0;
	}
	
/*	static count=0;
	count++;
	if(count==5) {
		alrm_handler.sa_handler = SIG_IGN;
		sigaction(SIGALRM,&alrm_handler,NULL);
		futex_up(&(getMember(*(mythread_runq()),block)));
		
	}*/


}

static void mythread_sighandler(int sig,siginfo_t *siginfo, void *ucp) {

	mythread_t self = mythread_self();
	if(sig == SIGALRM ) {

#ifdef VERBOSE
		printf("SIGALRM received in %ld\n",self->tid );
#endif
	    lll_lock(&resched_lock);
		if(mythread_tryenter_kernel() == TRUE ) {
	    lll_unlock(&resched_lock);
#ifdef VERBOSE
		printf("Signal handler entered kernel\n" );
#endif
			//set_resched_flag_all(mythread_runq());

			if( mythread_scheduler() ) {

/*
				if(mythread_inq(mythread_runq(),self) == TRUE) {
#ifdef VERBOSE
					printf("Thread : %ld preempting\n",self->tid);
#endif*/
					mythread_block(mythread_readyq(),BLOCKED);
/*

				} else
					mythread_leave_kernel_nonpreemptive();	
*/					
			} else {
#ifdef VERBOSE
				printf("Thread : %ld continueing to run\n",self->tid );
#endif
				mythread_leave_kernel_nonpreemptive();	
			}

		} else {
#ifdef VERBOSE
		printf("Signal handler failed to enter kernel\n" );
#endif
			resched_flag  = 1;



	    lll_unlock(&resched_lock);
		}
			
	} else if (sig == SIGUSR1) {
#ifdef VERBOSE
		printf("SIGUSR1 received in %ld\n",self->tid );
#endif
		resched_flag = 1;
	
		if ( !(self->state & BLOCKED) ) {	
			self->state  |= BLOCKED;		
			futex_down(&(self->block));
		}
	} else {
		print("Invalid Signal caught\n");
	}
	
//	printf("sigalrm\n");
/*	if((*mythread_readyq()!=NULL)&&(mythread_t)(*mythread_readyq())->item!=NULL)
		printf("%d : %d thread id is : %u\n",sig,count,((mythread_t)((*mythread_readyq())->item))->tid);
	printf("%d : %d thread id is : %u\n",sig,count,((mythread_t)((*mythread_runq())->item))->tid);

*/
/*
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
*/
}

void mythread_init_sched(void) {
	static int count =1;
	//gdbChk = count++;

//	printf("init sched called in %ld\n",mythread_self());	
	if(!INIT_SCHED) {
//		printf("init sched called in %ld\n",mythread_self());	
		alrm_struct=malloc(sizeof(struct itimerval));
		(alrm_struct->it_interval).tv_sec=0;
		(alrm_struct->it_interval).tv_usec=10000;
		(alrm_struct->it_value).tv_sec=0;
		(alrm_struct->it_value).tv_usec=10000;
		setitimer(ITIMER_REAL,alrm_struct,NULL);
#ifdef VERBOSE
		write(1,"init sched\n",strlen("init sched\n"));
#endif
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
		//sigprocmask(SIG_UNBLOCK,&alrm_handler.sa_mask,NULL);
		//mythread_exit(NULL);
	}
	//mythread_enter_kernel();
	//mythread_block(mythread_readyq(),BLOCKED);
	//write(1,"exit init sched\n",strlen("exit init sched\n"));
	
}

void mythread_exit_sched(void) {
#ifdef VERBOSE
	write(1,"exit sched\n",strlen("init sched\n"));
#endif
}



/*
*local low level lock based on compare and swap to ensure atomicity in 
*mythread_cond_wait();;
*/

static int lll_lock(int* lll_val) {
        while(1) {
                while(*lll_val!=1) {
                }
                        if(compare_and_swap(lll_val,0,1))  {
                                return;
                        }//spin here
        }
}
/*
*unlocks the low level lock for mythread_cond_wait();
*/
static int lll_unlock(int* lll_val) {
        *lll_val=1;
}

