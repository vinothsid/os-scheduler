#include "mythread.h"
#include <signal.h>
#include "myqueue.h"

int error_double_enq=0;
void mythread_q_init(mythread_queue_t *headp,void *item) {
	/*sigset_t set;
	sigemptyset(&set);
	sigaddset(&set,SIGUSR1);
	sigaddset(&set,SIGALRM);
	sigprocmask(SIG_BLOCK,&set,NULL);*/
	*headp=malloc(sizeof(struct mythread_queue));
	if((((mythread_t)item)->attribute)==NULL) {
                ((mythread_t)item)->attribute=malloc(sizeof(mythread_attr_t));
                ((mythread_t)item)->attribute->attr= DEFAULT_ATTR ;
        }
 
	(*headp)->item=item;
	(*headp)->next=NULL;
	(*headp)->prev=NULL;
	//sigprocmask(SIG_UNBLOCK,&set,NULL);
}

void mythread_enq(mythread_queue_t *headp,void *item) {
	/*sigset_t set;
	sigemptyset(&set);
	sigaddset(&set,SIGUSR1);
	sigaddset(&set,SIGALRM);
	sigprocmask(SIG_BLOCK,&set,NULL);*/
	int count=0;
	if(*headp==NULL) {
		mythread_q_init(headp,item);
		//sigprocmask(SIG_UNBLOCK,&set,NULL);
		return;
	}
	if(mythread_inq(headp,item)) {
		error_double_enq++;
		//write(1,"ERROR : enq of item already in q\n",strlen("ERROR : enq of item already in q\n"));
	}
	mythread_queue_t temp;
	temp=*headp;
	if((((mythread_t)item)->attribute)==NULL) {
		((mythread_t)item)->attribute=malloc(sizeof(mythread_attr_t));
		((mythread_t)item)->attribute->attr= DEFAULT_ATTR ;
	} 		
	while(temp->next !=NULL) {
/*
		if (((mythread_t)(temp->item))->attribute == NULL) {
	                ((mythread_t)(temp->item))->attribute=malloc(sizeof(mythread_attr_t));
	                ((mythread_t)(temp->item))->attribute->attr=99;

		}
*/		
		if((((mythread_t)(temp->item))->attribute)->attr > (((mythread_t)item)->attribute)->attr) {
		
			break;
		}
		else {
			temp=temp->next;
			count++;
		}						
	}
	
	if (((mythread_t)(temp->item))->attribute == NULL) {
                        ((mythread_t)(temp->item))->attribute=malloc(sizeof(mythread_attr_t));
                        ((mythread_t)(temp->item))->attribute->attr= DEFAULT_ATTR;

        }

	if((((mythread_t)(temp->item))->attribute)->attr > (((mythread_t)item)->attribute)->attr && (temp!=*headp)) {
		
		temp = temp->prev;
	}
	
	mythread_queue_t newNode=malloc(sizeof(struct mythread_queue));
	if(temp == *headp ) {
		if((((mythread_t)(temp->item))->attribute)->attr > (((mythread_t)item)->attribute)->attr) {
			newNode->item = item;
			newNode->next = *headp;
			(*headp)->prev = newNode;
			newNode->prev = NULL;
			*headp = newNode;
			//sigprocmask(SIG_UNBLOCK,&set,NULL);
			return;
		}
	}
	
	newNode->item=item;
	newNode->next=temp->next;
	newNode->prev=temp;			
	if(temp->next!=NULL)
		temp->next->prev=newNode;
	temp->next=newNode;
//	sigprocmask(SIG_UNBLOCK,&set,NULL);
	return;
}

int mythread_inq(mythread_queue_t *headp,void *item) {
	/*sigset_t set;
	sigemptyset(&set);
	sigaddset(&set,SIGUSR1);
	sigaddset(&set,SIGALRM);
	sigprocmask(SIG_BLOCK,&set,NULL);*/
	mythread_queue_t temp=*headp;
	while(temp!=NULL){
		if(temp->item==item) {
		//	sigprocmask(SIG_UNBLOCK,&set,NULL);
			return TRUE;
		}
		temp=temp->next;
	}
	//sigprocmask(SIG_UNBLOCK,&set,NULL);
	return FALSE;
}

void mythread_deq(mythread_queue_t* headp,void *item) {
/*	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set,SIGUSR1);
	sigaddset(&set,SIGALRM);
	sigprocmask(SIG_BLOCK,&set,NULL);*/
	mythread_queue_t temp=*headp;
        while(temp!=NULL){
                if(temp->item==item) {
			if(temp==*headp) {
				if((*headp)->next != NULL)
					(*headp)->next->prev=NULL;
				*headp=(*headp)->next;
				//free(temp);			
			}
			else {
				temp->prev->next=temp->next;
				if (temp->next != NULL)
					temp->next->prev=temp->prev;
				//free(temp);	
			}
			//sigprocmask(SIG_UNBLOCK,&set,NULL);
			return;
		}
                temp=temp->next;
        }
	//sigprocmask(SIG_UNBLOCK,&set,NULL);
	return;
}

void* mythread_deq_prio(mythread_queue_t *headp) {
/*	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set,SIGUSR1);
	sigaddset(&set,SIGALRM);
	sigprocmask(SIG_BLOCK,&set,NULL);*/
	mythread_queue_t temp=*headp;
	/*if((*headp)->next!=NULL) {
		(*headp)->next->prev=NULL;
       	} 
	*headp=(*headp)->next;*/
	//sigprocmask(SIG_UNBLOCK,&set,NULL);
	if(temp!=NULL)
		return temp->item;
	else
		return NULL;
}

int printQ(mythread_queue_t headp) {
	mythread_queue_t temp;
	temp = headp;
	int count=0;
	while(temp!=NULL) {
		count++;
		if(((mythread_t)temp->item)->attribute != NULL) 
			printf("contains ThreadID : %d '%d'\n",((mythread_t)temp->item)->tid,((mythread_t)temp->item)->attribute->attr);
		else 
			printf("contains ThreadID : %d '-1'\n",((mythread_t)temp->item)->tid);
			
			temp = temp->next;
	}

	printf("===========\n");
	return count;
}

/*
int main() {
	mythread_t t1=malloc(sizeof(struct mythread));
	mythread_t t2=malloc(sizeof(struct mythread));
	mythread_t t3=malloc(sizeof(struct mythread));
	mythread_t t4=malloc(sizeof(struct mythread));
	t1->tid=1;
	t2->tid=2;
	t3->tid=3;
	t4->tid=4;
	
	t1->attribute=malloc(sizeof(struct mythread_attr));
	t3->attribute=malloc(sizeof(struct mythread_attr));
	t4->attribute=malloc(sizeof(struct mythread_attr));
	t1->attribute->attr=3;
	t3->attribute->attr=3;
	t2->attribute=NULL;
	t4->attribute->attr=12;
	mythread_queue_t head=NULL;
	mythread_enq(&head,(void*)t1);
	mythread_enq(&head,(void*)t2);
	mythread_enq(&head,(void*)t3);
	mythread_enq(&head,(void*)t4);
	printQ(head);

	printf("After deque\n");
	mythread_deq(&head,(void *)t3);
	mythread_deq(&head,(void *)t1);

	printQ(head);
	mythread_deq(&head,(void *)t4);
	mythread_deq(&head,(void *)t2);

	printQ(head);
	mythread_enq(&head,(void*)t1);
	printQ(head);
}
*/
