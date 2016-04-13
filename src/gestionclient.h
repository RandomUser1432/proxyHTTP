//Prototype de fonction
int readmsg(int currentsocket, char *msg);
int readresponse(int currentsocket, char *response);
void readmsgHTTP(char *msg, char **request);
void gettypemsg(char *header, char *type);
int creategetsocket(char *hostname, char *port);
int searchHostname(char *hostname, char **hostnames);
int searchService(int service, int *services);