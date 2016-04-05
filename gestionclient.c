#include "common.h" 

int readmsg(int currentsocket, char *msg){
	int size = 0;
	
	memset(msg, 0, REQUESTSIZE);

	size=recv(currentsocket, msg, REQUESTSIZE, 0);

	return size;
}

int readresponse(int currentsocket, char *response){

	int size = 0;
		
	memset(response, 0, RESPONSESIZE);
	
	size=recv(currentsocket, response, REQUESTSIZE, 0);
	
	return size;
}

void readmsgHTTP(char *msg, char **request){
	
	int i = 0,j = 0, k = 0, h = 0;
	char buffer[REQUESTSIZE];
	
	memset(buffer, 0, REQUESTSIZE);
	
	printf("\n******HTTP Request******\n%s************************\n", msg);

	for (i = 0; i < REQUESTSIZE; i++){

		buffer[k] = msg[i];

		k++;

		if(msg[i] == '\n'){
			if (k == 1) buffer[k-1] = '\0';
			else buffer[k-2] = '\0';

			strcpy(request[j], buffer);		

			j++;
			memset(buffer, 0, REQUESTSIZE);
			k=0;
		}

	}

}

void gettypemsg(char *header, char *type){
	int i = 0;
	
	while(header[i] != ' ' && i < REQUESTSIZE){
		type[i] = header[i];	
		i++;
	}

	if (i >= REQUESTSIZE){
		perror("Premiere ligne d'une requete mal formee");
		exit(1);
	}

	type[i] = '\0';

}

int creategetsocket(char *hostname, char *port){
	
	int error, getsocket = 0;
	struct addrinfo hint, *cursor, *res;

	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;

	if ((error = getaddrinfo(hostname, port, &hint, &res)) < 0){
		perror("Probleme getaddrinfo socketget");
		printf("%s\n", gai_strerror(error));
		exit(1);
	}
	
	cursor = res;

	if((getsocket = socket(cursor->ai_family, cursor->ai_socktype, cursor-> ai_protocol)) < 0){
		printf("Problème initialisation socketget\n");
		exit(1);
	}

	if(connect (getsocket, cursor->ai_addr, cursor->ai_addrlen)  < 0){
		printf("Problème connect pour la socketget\n");
		exit(1);
	}

	printf("Le client souhaite se connecter au domaine : %s\n", hostname);

	freeaddrinfo(res);

	return getsocket;
	
}
