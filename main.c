#include "common.h"
#include "gestionclient.h"

void readSocket(int currentsocket, fd_set rset);

void usage(){
  printf("usage : servmulti numero_port_serveur\n");
}

int main(int argc, char **argv){

	if (argc != 2){
    		usage();
    		exit(1);
    	}
	//Probleme rencontre si il n'y a pas de service en argument

	int talksocket[FD_SETSIZE], getsocket[FD_SETSIZE], listensocket[2] = {0};
	int nblisten, maxfdp1, nbentree = 0;
	int i, j , k, length, size, currentsocket = 0;
	char *hostname, **request, buffinfo[ADDSIZE], msg[REQUESTSIZE], type[REQUESTSIZE], response[RESPONSESIZE];
	fd_set rset, pset;
	struct addrinfo hint, *res, *cursor;

	memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_PASSIVE;
	hint.ai_family = AF_INET6;
	hint.ai_socktype = SOCK_STREAM;
	//On initialise les champs de notre addrinfo selon les types de sockets desires (IPv4, IPv6, et TCP)
	//Le flag AI_PASSIVE indique que l'on desire des adresses de loopback

	request = (char **) malloc(MAXNBROW * sizeof(char*));

	for (i = 0; i < MAXNBROW; i++){
		request[i] = (char *) malloc(REQUESTSIZE * sizeof(char));
	}
	
	if(getaddrinfo(NULL, argv[1], &hint, &res) < 0){
		perror("Problème getaddrinfo");
		exit(1);
	}
	//getaddrinfo nous retourne les infos necessaires sous forme d'une liste chainees (res)

	FD_ZERO(&pset);
	FD_ZERO(&rset);
	FD_SET(0, &rset);
	//On initialise les structures fd_set pour mettre les bits des listensockets à 1 dans la boucle suivante

	for(cursor = res; cursor != NULL; cursor=cursor->ai_next){
		if((listensocket[nblisten] = socket(cursor->ai_family, cursor->ai_socktype, cursor-> ai_protocol)) < 0){
			printf("Problème initialisation socket numero: %d\n", nblisten+1);
			exit(1);
		}

		FD_SET(listensocket[nblisten], &rset);

		if(listensocket[nblisten] + 1 > maxfdp1){
			maxfdp1 = listensocket[nblisten] + 1;
		}

		if(bind(listensocket[nblisten], cursor->ai_addr, cursor->ai_addrlen) < 0){	//Survient si le numéros de port est < 1024
			printf("Problème bind pour la socket numero: %d\n", nblisten+1);
			exit(1);
		}

		if(listen(listensocket[nblisten], SOMAXCONN) != 0){
			printf("Problème listen pour la socket numero: %d\n", nblisten+1);
			exit(1);
		}

		nblisten++;
	}
	//Pour chaque addrinfo retourne (il y en a 2, une IPv4, l'autre IPv6), on instancie une socket d'ecoute

	freeaddrinfo(res);
	//On evite les fuites memoires en liberant les addrinfos

	for (i = 0; i < FD_SETSIZE; i++){
		talksocket[i] = -1;
		getsocket[i] = -1;
	}	

	while(1){

		pset = rset;	

		if((nbentree = select(maxfdp1, &pset, NULL, NULL, NULL)) < 0){
			perror("Problème select");
			exit(1);
		}
		//Le select nous informe de l'activite sur un descripteur(une socket par exemple)

		if(FD_ISSET(0, &pset)){
			memset(&msg, 0, sizeof(msg));
			read(0, msg, 3);
	
			msg[3] = '\0';

			if(strcmp(msg, "FIN") == 0){
				break;
			}

			nbentree--;

		}
		//On gere les donnees en stdin

		for (i=0; i < nblisten; i++){
	
			if(FD_ISSET(listensocket[i], &pset)){
				struct sockaddr_storage addrclient;				
				j = 0, length=sizeof(addrclient);
				
				while(( j < FD_SETSIZE) && (talksocket[j]) >= 0) j++;
				if (j == FD_SETSIZE){
					perror("Nombre maximum de client atteint");
					exit(1);
				}
				//On refuse les clients si le nb de descripteurs a atteint son maximum
				
				if((talksocket[j] = accept(listensocket[i], (struct sockaddr*)&addrclient, &length)) < 0){
					perror("Probleme d'instanciation d'une socket de dialogue");
					exit(1);
				}
				//Si cela est possible on accepte un nouveau client. La dialogue de socket est stocke dans un tableau

				if (talksocket[j] > maxfdp1-1){
					maxfdp1 = talksocket[j] + 1;
				}
				//Si necessaire, on incremente le nombre de descripteur a ecoute

				FD_SET(talksocket[j], &rset);
				//On demande l'ecoute de cette socket par le select

				getnameinfo((struct sockaddr *)&addrclient, length, buffinfo, sizeof(buffinfo), NULL, 0, NI_NUMERICHOST);

				printf("Le client %s s'est connecte\n", buffinfo);				
				//On recupere les infos du client est on affiche son IP

				nbentree--;

			}
		}
		//Ici on gere les activites sur les sockets d'ecoute (IPv4 et IPv6)

		k = 0; 

		while((nbentree >= 0) && (k < FD_SETSIZE)){
			
			if((currentsocket = getsocket[k]) >= 0 && FD_ISSET(currentsocket, &pset)){
				
				size = readmsg(currentsocket, response);
				//On lit les donnees sur le socket

				if(size < 0){
					perror("Problème lecture getsocket");
					exit(1);
				}else if (size == 0){
					close(currentsocket);
					getsocket[k] = -1;
					FD_CLR(currentsocket, &rset);
				}else{					
					printf("\n********HTTP Response********\n");
					printf("%s\n", response);	
					printf("************************\n");	
					send(talksocket[k], response, size, 0);
					//On envoit la response HTTP au client correspondant
				}		

				nbentree--;
			}
			//On gere l'activite sur les sockets connectees aux serveurs HTTP/HTTPS

			if(((currentsocket = talksocket[k]) >=0) && FD_ISSET(currentsocket, &pset)){
				
				size = readmsg(currentsocket, msg);
				//On lit les donnees sur la socket de dialogue
				
				if(size < 0){
					perror("Problème lecture talksocket");
					exit(1);
				}else if(size == 0){
					close(currentsocket);
					talksocket[k] = -1;
					FD_CLR(currentsocket, &rset);
				}else{
					readmsgHTTP(msg, request);
					//On adapte la request HTTP/HTTPS a notre structure de donnees					

					gettypemsg(request[0], type);
					//On obtient le type de request HTTP

					printf("Le type de requête est : %s\n", type);
	
					if (strcmp(type,"GET") == 0){
						hostname = request[1]+6;
						
						/*					

						if (getsocket[k] != -1){
							close(getsocket[k]);
							FD_CLR(getsocket[k], &rset);
						}

						*/
						getsocket[k] = creategetsocket(hostname, "80");
						//Si le request est de type GET on cree une socket vers le serveur HTTP correspondant au hostname

						if (getsocket[k] > maxfdp1-1){
							maxfdp1 = getsocket[k] + 1;
						}
						FD_SET(getsocket[k], &rset);
						//On demande la surveillance de la socket par le select
					
						send(getsocket[k], msg, size, 0);
						//On envoie le request HTTP de notre client au serveur demande

					} else if (strcmp(type,"CONNECT") == 0) {


						char *inter = request[4] + 6;
						int taille = strlen(inter) - 4;
						hostname = (char *) malloc(taille * sizeof(char));
						memcpy(hostname, &inter[0],taille);
						//hostname[taille] = '\0';
						printf("%s\n", hostname);

						getsocket[k] = creategetsocket(hostname,"443");
						//Si la requéte est de type CONNECT, on crée une socket vers le serveur HTTPS correspondant au hostname

						if (getsocket[k] < maxfdp1 - 1) {
							maxfdp1 = getsocket[k] + 1;
						}
						FD_SET(getsocket[k], &rset);
						//On demande la surveillance de la socket par le select
					
						send(getsocket[k], msg, size, 0);
						//On envoie le request HTTPS de notre client au serveur demande


					}else{
						send(getsocket[k], msg, size, 0);
					}

				}						

				nbentree--;
				
			}
			//On gere l'activite sur les sockets de dialogue  

			k++;

			
		}
		//On gere les sockets de dialogue avec les serveurs HTTP/HTTPS et nos clients proxy

/*
		int test = -1;
		if(nbentree != 0){

			test = select(maxfdp1, &pset, NULL, NULL, NULL);
			printf("La valeur de retour du select est de %d\n", test);

			int l = 0;
			while (l < FD_SETSIZE) {
				if (getsocket[l] != -1) {
					printf("la socket numéro %d à la case %d n'est pas traitée\n", getsocket[l],l);
				}
				l++;
			}

			perror("Probleme avec select: nombre d'entree non correcte");
			printf("Il reste encore %d entree(s)\n", nbentree);
			exit(1);
		}
		//On renvoie une erreur si tous les descripteurs n'ont pas ete traites
*/		

	}
	//On boucle que le traitement des donnees recu par les sockets
	

	// fermer toutes les sockets
	
	exit(0);
}
