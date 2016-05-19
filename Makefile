all: bankserver bankclient

bankserver: tcpserver.c tcpserver.h libserv.a
	gcc -o bankserver tcpserver.c tcpserver.h libserv.a -Wall -Werror -g -pthread

bankclient: tcpclient.c
	gcc -o bankclient tcpclient.c -g -Wall -Werror -pthread

libserv.a: tokenizer.o hashtable.o
	ar -r libserv.a tokenizer.o hashtable.o 

tokenizer.o: tokenizer.c tokenizer.h
	gcc -c tokenizer.c tokenizer.h -Wall -Werror -g

hashtable.o: hashtable.c hashtable.h
	gcc -c hashtable.c hashtable.h -Wall -Werror -g




clean: 
	rm -r -f bankclient bankserver Makefile~ libserv.a tokenizer.h.gch hashtable.h.gch tokenizer.o hashtable.o tokenizer.c~ hashtable.c~ tokenizer.h~ hashtable.h~ tcpserver.c~ tcpserver.h~ tcpclient.h~ tcpclient.c~ readme.pdf~ testplan.txt~ testcases.txt~