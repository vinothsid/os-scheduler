all:
	gcc -g -c myqueue.c -o myqueue.o
	gcc -g -c mysched.c -o mysched.o
	gcc -g mysched.o myqueue.o mytest.c mythread-new3.a -o mytest 
#	gcc -g mysched.o myqueue.o mytest1.c mythread-new3.a -o mytest 
clean:
	rm mytest 
