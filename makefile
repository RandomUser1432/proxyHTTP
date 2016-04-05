proxyHTTP: main.o gestionclient.o common.h gestionclient.h
	gcc -o proxyHTTP main.o gestionclient.o common.h

main.o:	main.c common.h gestionclient.h
	gcc -c main.c common.h gestionclient.h

gestionclient.o: gestionclient.c common.h gestionclient.h
	gcc -c gestionclient.c common.h gestionclient.h

clear:
	rm -rf *.o proxyHTTP
