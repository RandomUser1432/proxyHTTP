proxyHTTP: main.o gestionclient.o common.h
	gcc -o proxyHTTP main.o gestionclient.o common.h

main.o:	main.c
	gcc -c main.c

gestionclient.o: gestionclient.c
	gcc -c gestionclient.c

clear:
	rm *.o proxyHTTP *~
