proxyHTTP: bin main.o gestionclient.o
	gcc -o ./proxyHTTP ./bin/main.o ./bin/gestionclient.o

bin: 
	mkdir -p bin

main.o:	./src/main.c ./src/common.h ./src/gestionclient.h
	gcc -Wall -Werror -Wextra -c ./src/main.c -o ./bin/main.o

gestionclient.o: ./src/gestionclient.c ./src/common.h ./src/gestionclient.h
	gcc -Wall -Werror -Wextra -c ./src/gestionclient.c -o ./bin/gestionclient.o

clear:
	rm -rf ./bin proxyHTTP
