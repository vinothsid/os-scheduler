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

void print_queues() {
        print("RunQ\n");
        printQ(*(mythread_runq()));
        print("ReadyQ\n");
        printQ(*(mythread_readyq()));

}
static void mythread_sighandler(int sig,siginfo_t *siginfo, void *ucp) {
	mythread_t self=mythread_self();
	if(sig==SIGALRM) {
#ifdef VERBOSE
                printf("SIGALRM received in %ld\n",self->tid );
#endif

		self->state|=ALARM;
		self->reschedule=1;
	} else {
#ifdef VERBOSE
                printf("SIGALRM received in %ld\n",self->tid );
#endif

		self->state|=SIGUSR;
		self->reschedule=1;
	}
	if(mythread_tryenter_kernel()==TRUE) {
#ifdef VERBOSE
                printf("Signal handler entered kernel\n" );
#endif

		if(sig==SIGALRM) {
			mythread_queue_t ptr=*mythread_runq();
			while(ptr!=NULL) {
				if(getMember(ptr,tid) != self->tid) {
#ifdef VERBOSE
				printf("Sending SIGUSR to %ld\n",getMember(ptr,tid) );
	#endif
					syscall(SYS_tgkill,getpid(),getMember(ptr,tid),SIGUSR1);

				}
				ptr = ptr->next;
			}
	
		}
		if(mythread_scheduler()==TRUE) {

			mythread_queue_t* readyq=mythread_readyq();
			mythread_block(readyq,BLOCKED);
		} else {
			mythread_leave_kernel_nonpreemptive();
		}

	} 

}



void mythread_leave_kernel(void) {
//	mythread_leave_kernel_nonpreemptive();
	mythread_t self=mythread_self();
	int exitflag;
	do {
		exitflag=0;
		if(self->reschedule == 1) {
			if(self->state & ALARM) {
				mythread_queue_t ptr=*mythread_runq();
				while(ptr!=NULL) {
					if(getMember(ptr,tid) != self->tid) {
	#ifdef VERBOSE
					printf("LK: Sending SIGUSR to %ld\n",getMember(ptr,tid) );
		#endif
						syscall(SYS_tgkill,getpid(),getMember(ptr,tid),SIGUSR1);

					}
					ptr = ptr->next;
				}
		
			}
			
			if(mythread_scheduler() == TRUE) {
				mythread_queue_t* readyq=mythread_readyq();
                 		mythread_block(readyq,BLOCKED);
			} else {

				mythread_leave_kernel_nonpreemptive();
			}
		} else {
			mythread_queue_t ptr=*mythread_runq();
			while(ptr!=NULL) {
				if(getMember(ptr,tid) != self->tid && getMember(ptr,reschedule) == 1) {
#ifdef VERBOSE
			printf("Sending SIGUSR to reschedulable outside of sigalrm condition %ld\n",getMember(ptr,tid) );
#endif
					syscall(SYS_tgkill,getpid(),getMember(ptr,tid),SIGUSR1);

				}
				ptr = ptr->next;
			}
#ifdef DEBUG
                                //print("SH: Scheduler returns true\n");
                                print_queues();
#endif   
			mythread_leave_kernel_nonpreemptive();
		}
		if(self->reschedule!=0) {
			if(mythread_tryenter_kernel() == TRUE) {
				//self->reschedule=0;
				exitflag=1;
			}
		}

	} while(exitflag == 1);
}

static int mythread_scheduler(void) {
	mythread_t self=mythread_self();
	self->state &=~ALARM;
	self->state &=~SIGUSR;
	self->reschedule=0;
	if(*mythread_readyq()!=NULL && mythread_inq(mythread_runq(),self)) {

		mythread_t highPrio = mythread_deq_prio(mythread_readyq());
		if( self->attribute->attr < highPrio->attribute->attr ) 
			return 0;
		else
			return 1;
	} else {
		return 0;
	}
}


void mythread_init_sched(void) {
	static int count =1;

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
 * pthread_attr_init - initialize a thread attributes object attr
 * with the default value for all of the individual attributes used
 * by a given implementation.
 */
int mythread_attr_init(mythread_attr_t *attr) {
	if(attr != NULL) {
		attr->attr = DEFAULT_ATTR ;
		return 0;
	} else {
		return -1;
	}
	

}

/*
 * pthread_attr_destroy - destroy a  thread  attributes object
 */
int mythread_attr_destroy(mythread_attr_t *attr) {
	if(attr != NULL) {
		attr->attr = DEFAULT_ATTR ;
		return 0;
	} else {
		return -1;
	}
	
}

/*
 * pthread_attr_getschedparam - get the scheduling parameter
 * attributes in the attr argument
 */
int mythread_attr_getschedparam(const mythread_attr_t *attr,
                struct sched_param *param) {
	if(attr!=NULL) {
		param->__sched_priority = attr->attr;
		return 0;
	} else {
		return -1;
	}
}

/*
 * pthread_attr_setschedparam - set the scheduling parameter
 * attributes in the attr argument
 */
int mythread_attr_setschedparam(mythread_attr_t *attr,const struct sched_param *param) {

	if(attr != NULL && param != NULL && param->__sched_priority >=0 && param->__sched_priority < 100) {
		attr->attr = param->__sched_priority ;
		return 0;	
	} else {
		return -1;
	}
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

